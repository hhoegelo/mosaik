#pragma once

namespace Dsp
{
  namespace Api
  {
    namespace Realtime
    {
      class Api;
    }
  }
}

namespace AudioMidi
{
  class AudioMidi
  {
   public:
    AudioMidi(Dsp::Api::Realtime::Api &dsp);
  };
}