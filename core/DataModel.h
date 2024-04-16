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
    DataModel()
    {
      for(auto c = 0; c < NUM_TILES; c++)
        tiles[c].id = c;

      tiles[0].selected = true;
    }

    struct Tile
    {
      TileId id;

      Tools::ReactiveVar<Path> sample { "" };
      Tools::ReactiveVar<std::array<bool, NUM_STEPS>> pattern { {} };
      Tools::ReactiveVar<float> gain { 0.f };
      Tools::ReactiveVar<float> balance { 0.f };
      Tools::ReactiveVar<float> shuffle { 0.f };
      Tools::ReactiveVar<bool> muted { false };
      Tools::ReactiveVar<bool> reverse { false };
      Tools::ReactiveVar<bool> selected { false };

      Tools::ReactiveVar<FramePos> envelopeFadeInPos { 0 };
      Tools::ReactiveVar<FramePos> envelopeFadedInPos { 0 };
      Tools::ReactiveVar<FramePos> envelopeFadeOutPos { std::numeric_limits<FramePos>::max() };
      Tools::ReactiveVar<FramePos> envelopeFadedOutPos { std::numeric_limits<FramePos>::max() };
      Tools::ReactiveVar<FramePos> triggerFrame { 0 };

      Tools::ReactiveVar<float> speed { 0 };

      struct Wizard
      {

        Tools::ReactiveVar<uint8_t> mode { WizardMode::Or };
        Tools::ReactiveVar<float> rotate { 0 };
        Tools::ReactiveVar<float> ons { 1 };
        Tools::ReactiveVar<float> offs { 3 };
      } wizard;
    };

    struct Globals
    {
      Tools::ReactiveVar<float> tempo { 120.f };
      Tools::ReactiveVar<float> volume { 0 };
    };

    Globals globals;
    std::array<Tile, NUM_TILES> tiles;
    Tools::ReactiveVar<Path> prelistenSample { "" };
    Tools::ReactiveVar<uint8_t> prelistenInteractionCounter { 0 };
  };
}
