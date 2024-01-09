#include "AlsaIn.h"
#include <alsa/asoundlib.h>
#include <iostream>

#define checkEQ(A, B)                                                                                                  \
  if(B == A)                                                                                                           \
  std::cerr << "Check Failed: " << #B << "==" << #A << std::endl

#define checkAlsa(A)                                                                                                   \
  if(auto res = A)                                                                                                     \
  std::cerr << "Alsa Midi Error: " << #A << " throws error: " << snd_strerror(res) << std::endl

namespace Midi
{

  AlsaIn::AlsaIn(const std::string &device, std::function<void(const MidiEvent &)> cb)
  {
    checkEQ(pipe(m_cancelPipe), 0);

    m_bg = std::async(std::launch::async,
                      [this, device = device, cb = std::move(cb)]
                      {
                        snd_rawmidi_t *handle = nullptr;
                        snd_rawmidi_open(&handle, nullptr, device.c_str(), SND_RAWMIDI_NONBLOCK);

                        if(handle)
                        {
                          while(!m_quit)
                            readMidi(handle);

                          snd_rawmidi_close(handle);
                        }
                      });
  }

  AlsaIn::~AlsaIn()
  {
    m_quit = true;
    uint8_t v = 0;
    checkEQ(write(m_cancelPipe[1], &v, 1), 1);
    m_bg.wait();
  }

  struct MidiEventCodecs
  {
    MidiEventCodecs(unsigned encoderSize, unsigned decoderSize)
    {
      snd_midi_event_new(encoderSize, &encoder);
      snd_midi_event_new(decoderSize, &decoder);
      snd_midi_event_no_status(decoder, 1);  // force full-qualified midi events (no "running status")
    }

    ~MidiEventCodecs()
    {
      snd_midi_event_free(encoder);
      snd_midi_event_free(decoder);
    }

    snd_midi_event_t *encoder = nullptr;
    snd_midi_event_t *decoder = nullptr;
  };

  void AlsaIn::readMidi(snd_rawmidi_t *handle)
  {
    // Raw bytes buffer for snd_raw_midi_read().
    // For large SysEx data coming in fast a fair amount of background buffer is really required
    constexpr unsigned RAWMIDI_BUF_SIZE = 131072;  // empirical value + safety margin

    // Maximum granularity of the main receive loop.
    // Typically, incoming data between polls will be in much smaller sized blocks.
    constexpr unsigned LOCAL_BUF_SIZE = 1024;  // non-critical value

    uint8_t buffer[LOCAL_BUF_SIZE];
    size_t bufferReserve = RAWMIDI_BUF_SIZE;

    int numPollFDs = snd_rawmidi_poll_descriptors_count(handle);
    pollfd pollFileDescriptors[numPollFDs + 1];
    numPollFDs = snd_rawmidi_poll_descriptors(handle, pollFileDescriptors, numPollFDs);
    pollFileDescriptors[numPollFDs].fd = m_cancelPipe[0];
    pollFileDescriptors[numPollFDs].events = POLLIN;

    snd_rawmidi_nonblock(handle, 1);  // make reads non-blocking

    snd_rawmidi_params_t *pParams;
    snd_rawmidi_params_alloca(&pParams);
    snd_rawmidi_params_current(handle, pParams);
    snd_rawmidi_params_set_buffer_size(handle, pParams, RAWMIDI_BUF_SIZE);
    snd_rawmidi_params(handle, pParams);
    snd_rawmidi_params_current(handle, pParams);

    snd_rawmidi_status_t *pStatus;
    snd_rawmidi_status_alloca(&pStatus);

    MidiEventCodecs codecs(LOCAL_BUF_SIZE, 128);

    while(!m_quit)
    {
      checkAlsa(snd_rawmidi_status(handle, pStatus));

      if(snd_rawmidi_status_get_xruns(pStatus) > 0)
      {
        std::cerr << "raw_midi receive buffer overrun";
        snd_midi_event_reset_decode(codecs.decoder);  // purge any half-decoded events
      }

      if(auto result = fetchChunk(handle, buffer, LOCAL_BUF_SIZE, numPollFDs + 1, pollFileDescriptors))
        processChunk(buffer, result, codecs.encoder, codecs.decoder);
    }
  }

  size_t AlsaIn::fetchChunk(snd_rawmidi_t *handle, uint8_t *buffer, size_t length, int numPollFDs,
                            pollfd *pollFileDescriptors)
  {
    auto result = snd_rawmidi_read(handle, buffer, length);
    if(result == -EAGAIN)  // nothing remaining or no data
    {
      // now sleep until data is coming in ...
      if(poll(pollFileDescriptors, numPollFDs, -1) > 0)
        return 0;

      std::cerr << "Polling the midi input file descriptor failed." << std::endl;
    }

    if(result < 0)  // catch errors
      std::cerr << "Error reading midi: " << snd_strerror(static_cast<int>(result)) << std::endl;

    return result;
  }

  void AlsaIn::processChunk(uint8_t *buffer, size_t length, snd_midi_event_t *encoder, snd_midi_event_t *decoder)
  {
    snd_seq_event_t event;

    while(length > 0)
    {
      auto consumed = snd_midi_event_encode(encoder, buffer, length, &event);

      if(consumed <= 0)
      {
        std::cerr << "Could not encode stream into midi event =>" << snd_strerror(consumed) << std::endl;
        snd_midi_event_reset_encode(encoder);
        return;
      }

      length -= consumed;
      buffer += consumed;

      if(event.type < SND_SEQ_EVENT_SONGPOS)
      {
        // event is complete *and* is relevant for us --> reconvert event to 1..3 bytes raw data
        snd_midi_event_reset_decode(decoder);
        Midi::SimpleMessage msg;
        msg.numBytesUsed = std::min(3l, snd_midi_event_decode(decoder, msg.rawBytes.data(), 3, &event));
        send(EndPoint::ExternalMidiOverIPClient, msg);
      }
    }
  }

}