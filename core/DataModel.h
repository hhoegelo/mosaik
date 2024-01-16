#pragma once

#include <stdint.h>
#include <filesystem>
#include <array>

namespace Core
{
  enum class Direction
  {
    Forward = 1,
    Backward = -1
  };

  struct DataModel
  {
    explicit DataModel(const std::filesystem::path &f);
    ~DataModel();

    struct Channel
    {
      std::filesystem::path sample {};
      std::array<bool, NUM_STEPS> pattern {};
      float gain = 1.f;
      float balance = 0.f;
      bool muted = false;
      bool reverse = false;
    };

    std::array<Channel, NUM_CHANNELS> channels;

    float tempo = 120.f;
    float volume = 1.f;

    std::filesystem::path backing;
  };
}
