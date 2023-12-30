#include "AudioMidi.h"
#include "AlsaOut.h"

namespace AudioMidi
{
  AudioMidi::AudioMidi(Dsp::Api::Realtime::Interface &dsp, const std::string &alsa_out)
      : m_alsaOut(std::make_unique<AlsaOut>(dsp, alsa_out))
  {
  }

  AudioMidi::~AudioMidi() = default;
}