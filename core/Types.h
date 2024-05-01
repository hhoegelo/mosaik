#pragma once

#include <dsp/Types.h>
#include <filesystem>
#include <variant>
#include <optional>

namespace Core
{
  enum WizardMode
  {
    Or = 0,
    And = 1,
    Replace = 2,
    Not = 3

  };
  enum class ParameterId
  {
    // Globals
    GlobalTempo,
    GlobalVolume,

    // Per Tile
    Selected,
    SampleFile,
    Shuffle,
    Pattern,
    Balance,
    Gain,
    Mute,
    Reverse,
    EnvelopeFadeInPos,
    EnvelopeFadedInPos,
    EnvelopeFadeOutPos,
    EnvelopeFadedOutPos,
    TriggerFrame,
    Speed,

    // Per Tile Playground
    Playground1,
    Playground2,
    Playground3,
    Playground4,
    Playground5,
    Playground6,
    Playground7,

    // Main Playground
    MainPlayground1,
    MainPlayground2,
    MainPlayground3,
    MainPlayground4,
    MainPlayground5,
    MainPlayground6,
    MainPlayground7,

    // Wizard
    WizardMode,
    WizardRotate,
    WizardOns,
    WizardOffs,
  };

  using Path = std::filesystem::path;
  using Pattern = std::array<bool, NUM_STEPS>;
  using Float = float;
  using Bool = bool;
  using TileId = std::optional<uint32_t>;
  using FramePos = int64_t;

  using ParameterValue = std::variant<Bool, Float, Path, Pattern, FramePos, uint8_t>;
}
