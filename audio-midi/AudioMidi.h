#pragma once

#include <string>
#include <memory>

namespace Dsp
{
  namespace Api
  {
    namespace Realtime
    {
      class Interface;
    }
  }
}

namespace AudioMidi
{
  class AlsaOut;

  class AudioMidi
  {
   public:
    AudioMidi(Dsp::Api::Realtime::Interface &dsp, const std::string &alsa_out);
    ~AudioMidi();

   private:
    std::unique_ptr<AlsaOut> m_alsaOut;
  };
}