#pragma once

#include <stdint.h>
#include <filesystem>

namespace Core
{
  enum class Direction
  {
    Forward = 1,
    Backward = -1
  };

  struct DataModel
  {
    struct Channel
    {
      std::filesystem::path sample {};
      std::array<bool, NUM_STEPS> pattern {};
      float gain = 1.f;
      float balance = 0.f;
      bool muted = false;
      Direction playbackDirection;
    };

    Channel channels[NUM_CHANNELS];

    float tempo = 120.f;
    float volume = 1.f;
  };

}