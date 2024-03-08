#pragma once

#include <core/Types.h>
#include <cstdint>
#include <filesystem>
#include <array>
#include <set>

namespace Core
{
  struct DataModel
  {
    explicit DataModel(std::filesystem::path f);
    ~DataModel();

    struct Tile
    {
      TileId id;

      std::filesystem::path sample {};
      std::array<bool, NUM_STEPS> pattern {};
      float gain = 1.f;
      float balance = 0.f;
      bool muted = false;
      bool reverse = false;
      bool selected = false;
    };

    struct Globals
    {
      float tempo = 120.f;
      float volume = 1.f;
      float shuffle = 0.5f;
    };

    Globals globals;
    std::array<Tile, NUM_TILES> tiles;

    std::filesystem::path backing;
  };
}
