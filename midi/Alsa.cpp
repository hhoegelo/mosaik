#include "Alsa.h"
#include <alsa/asoundlib.h>
#include <iostream>
#include <utility>
#include <glibmm/main.h>
#include <giomm/unixinputstream.h>

#define checkAlsa(A)                                                                                                   \
  if(auto res = A)                                                                                                     \
  std::cerr << "Alsa Midi Error: " << #A << " throws error: " << snd_strerror(res) << std::endl

namespace Midi
{

  Alsa::Alsa(const std::string &device, Callback cb)
      : m_cb(std::move(cb))
  {
    snd_midi_event_new(128, &m_encoder);
    snd_midi_event_new(128, &m_decoder);
    snd_midi_event_no_status(m_decoder, 1);

    checkAlsa(snd_rawmidi_open(&m_input, &m_output, device.c_str(), SND_RAWMIDI_NONBLOCK));
    snd_rawmidi_nonblock(m_input, 1);

    pollfd polls[128];
    snd_rawmidi_poll_descriptors(m_input, polls, 128);

    auto channel = g_io_channel_unix_new(polls[0].fd);

    m_id = g_io_add_watch(
        channel, (GIOCondition) (G_IO_IN | G_IO_HUP | G_IO_ERR),
        +[](GIOChannel *source, GIOCondition condition, gpointer data) -> gboolean
        {
          if(condition & G_IO_IN)
          {
            auto pThis = static_cast<Alsa *>(data);
            pThis->processInput();
          }
          return true;
        },
        this);
    g_io_channel_unref(channel);
  }

  Alsa::~Alsa()
  {
    g_source_remove(m_id);
    snd_rawmidi_close(m_input);
    snd_rawmidi_close(m_output);
  }

  void Alsa::processInput()
  {
    snd_seq_event_t event;
    uint8_t byte;

    while(true)
    {
      auto readResult = snd_rawmidi_read(m_input, &byte, 1);

      if(readResult == 1)
      {
        if(snd_midi_event_encode_byte(m_encoder, byte, &event) == 1)
        {
          if(event.type != SND_SEQ_EVENT_NONE)
          {
            MidiEvent e;
            snd_midi_event_decode(m_decoder, e.data(), e.size(), &event);
            m_cb(e);
          }
        }
      }
      else
        return;
    }
  }

  void Alsa::send(const Alsa::MidiEvent &event)
  {
    if(m_output)
      snd_rawmidi_write(m_output, event.data(), event.size());
  }

  void Alsa::send(const uint8_t &event)
  {
    if(m_output)
      snd_rawmidi_write(m_output, &event, 1);
  }
}