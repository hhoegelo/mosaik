#pragma once

#include <string>
#include <alsa/asoundlib.h>
#include <functional>

namespace Midi
{
  class AlsaIn
  {
   public:
    using MidiEvent = std::array<uint8_t, 3>;
    using Callback = std::function<void(const MidiEvent &)>;

    AlsaIn(const std::string &device, const Callback &cb);
    ~AlsaIn();

   private:
    snd_rawmidi_t *m_device = nullptr;
    unsigned int m_id = -1;
    Callback m_cb;
  };
}
