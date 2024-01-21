#pragma once

#include <dsp/Types.h>
#include <filesystem>

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
        virtual SharedSampleBuffer getSamples(const std::filesystem::path &path) const = 0;
      };
    }
  }
}