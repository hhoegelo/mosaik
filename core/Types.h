#pragma once

#include <dsp/Types.h>
#include <filesystem>
#include <variant>
#include <optional>
#include <vector>

namespace Core
{
  constexpr auto c_silenceDB = Dsp::c_silenceDB;
  constexpr auto c_zeroDB = Dsp::c_zeroDB;
  constexpr auto c_maxDB = Dsp::c_maxDB;

  enum class ParameterId
  {
    Unused,

    // Globals
    GlobalTempo,    // float 20..240 bpm
    GlobalVolume,   // float 0...1
    GlobalShuffle,  // float 0...1

    // Per Tile
    Selected,            // bool => is the given tile selected?
    SampleFile,          // std::filesystem::path
    Pattern,             // std::array<bool, NUM_STEPS>
    Balance,             // float -1...1
    Gain,                // float 0...1
    Mute,                // bool
    Reverse,             // bool
    EnvelopeFadeInPos,   // FadePos
    EnvelopeFadeInLen,   // FadeLen
    EnvelopeFadeOutPos,  // FadePos
    EnvelopeFadeOutLen,  // FadeLen
    Speed,               // float -1...1
  };

  using Path = std::filesystem::path;
  using Pattern = std::array<bool, NUM_STEPS>;
  using Float = float;
  using Bool = bool;
  using TileId = std::optional<uint32_t>;
  using FramePos = int64_t;

  using ParameterValue = std::variant<Bool, Float, Path, Pattern, FramePos>;
}