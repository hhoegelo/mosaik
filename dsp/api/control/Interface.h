#pragma once

namespace Dsp
{
  class AudioKernel;
  
  namespace Api
  {
    namespace Control
    {
      class Interface
      {
       public:
        virtual ~Interface() = default;

        virtual void takeAudioKernel(AudioKernel *kernel) = 0;
      };
    }
  }
}