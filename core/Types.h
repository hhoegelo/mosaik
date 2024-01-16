#pragma once

#pragma once

#include <tools/SignalSlot.h>
#include <filesystem>
#include <map>
#include <functional>
#include <variant>
#include <optional>

namespace Core
{
  enum class ParameterId
  {
    GlobalTempo,   // float 20..240 bpm
    GlobalVolume,  // float 0...1

    SampleFile,  // std::filesystem::path
    Pattern,     // std::array<bool, NUM_STEPS>
    Balance,     // float -1...1
    Gain,        // float 0...1
    Mute,        // bool
    Reverse,     // bool
  };

  using Path = std::filesystem::path;
  using Pattern = std::array<bool, NUM_STEPS>;
  using Float = float;
  using Bool = bool;
  using ParameterValue = std::variant<Bool, Float, Path, Pattern>;
  using TileId = std::optional<uint32_t>;
}