#pragma once

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
  class AudioMidi
  {
   public:
    AudioMidi(Dsp::Api::Realtime::Interface &dsp);
  };
}