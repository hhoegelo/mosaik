#pragma once

#include <string>
#include <alsa/asoundlib.h>
#include <functional>

namespace Midi
{
  class Alsa
  {
   public:
    using MidiEvent = std::array<uint8_t, 3>;
    using Callback = std::function<void(const MidiEvent &)>;

    Alsa(const std::string &device, Callback cb);
    ~Alsa();

    void send(const MidiEvent &event);
    void send(const uint8_t &event);

   private:
    void processInput();

    snd_rawmidi_t *m_input = nullptr;
    snd_rawmidi_t *m_output = nullptr;
    unsigned int m_id = -1;
    Callback m_cb;

    snd_midi_event_t *m_encoder = nullptr;
    snd_midi_event_t *m_decoder = nullptr;
  };
}
