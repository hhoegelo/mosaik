#pragma once

#include <dsp/Types.h>
#include <stdint.h>
#include <string>
#include <memory>
#include <filesystem>
#include <vector>
#include <array>

namespace Dsp
{
  namespace Api
  {
    namespace Control
    {
      struct AudioKernel
      {
        float volume = 0.0;
        uint32_t framesPer16th = 1;

        struct Channel
        {
          std::shared_ptr<std::vector<StereoFrame>> audio { std::make_shared<std::vector<StereoFrame>>() };
          std::array<bool, NUM_STEPS> pattern {};
          float gainLeft { 0.0f };
          float gainRight { 0.0f };
          int8_t playbackFrameIncrement { 0 };
        };

        Channel channels[NUM_CHANNELS];
      };

      class Interface
      {
       public:
        virtual ~Interface() = default;

        virtual void takeAudioKernel(AudioKernel *kernel) = 0;
      };
    }
  }
}