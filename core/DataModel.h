#pragma once

#include <tools/ReactiveVar.h>

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
    DataModel();
    ~DataModel();

    struct Tile
    {
      TileId id;

      Tools::ReactiveVar<std::filesystem::path> sample { "" };
      Tools::ReactiveVar<std::array<bool, NUM_STEPS>> pattern { {} };
      Tools::ReactiveVar<float> gain { 1.f };
      Tools::ReactiveVar<float> balance { 0.f };
      Tools::ReactiveVar<bool> muted { false };
      Tools::ReactiveVar<bool> reverse { false };
      Tools::ReactiveVar<bool> selected { false };

      Tools::ReactiveVar<FramePos> envelopeFadeInPos { 0 };
      Tools::ReactiveVar<FramePos> envelopeFadeInLen { 0 };
      Tools::ReactiveVar<FramePos> envelopeFadeOutPos { std::numeric_limits<FramePos>::max() };
      Tools::ReactiveVar<FramePos> envelopeFadeOutLen { 0 };

      Tools::ReactiveVar<float> speed { 0 };
    };

    struct Globals
    {
      Tools::ReactiveVar<float> tempo { 120.f };
      Tools::ReactiveVar<float> volume { 1.f };
      Tools::ReactiveVar<float> shuffle { 0.5f };
    };

    Globals globals;
    std::array<Tile, NUM_TILES> tiles;

    std::filesystem::path backing;
  };
}
