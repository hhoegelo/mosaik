#pragma once

#include <stdint.h>
#include <filesystem>
#include <array>

namespace Core
{
  struct DataModel
  {
    explicit DataModel(const std::filesystem::path &f);
    ~DataModel();

    struct Tile
    {
      std::filesystem::path sample {};
      std::array<bool, NUM_STEPS> pattern {};
      float gain = 1.f;
      float balance = 0.f;
      bool muted = false;
      bool reverse = false;
    };

    std::array<Tile, NUM_TILES> tiles;

    float tempo = 120.f;
    float volume = 1.f;

    std::filesystem::path backing;
  };
}
