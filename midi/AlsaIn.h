#pragma once

#include <string>
#include <functional>
#include <future>
#include <alsa/asoundlib.h>

namespace Midi
{
  class MidiEvent;

  class AlsaIn
  {
   public:
    AlsaIn(const std::string &device, std::function<void(const MidiEvent &)> cb);
    ~AlsaIn();

   private:
    void readMidi(snd_rawmidi_t *handle);
    void processChunk(uint8_t *buffer, size_t length, snd_midi_event_t *encoder, snd_midi_event_t *decoder);
    size_t fetchChunk(snd_rawmidi_t *handle, uint8_t *buffer, size_t length, int numPollFDs,
                      pollfd *pollFileDescriptors);

    int m_cancelPipe[2];
    bool m_quit = false;
    std::future<void> m_bg;
  };
}
