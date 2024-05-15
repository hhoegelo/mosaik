#pragma once

#include <dsp/Types.h>
#include <tools/json.h>
#include <filesystem>
#include <variant>
#include <optional>

namespace Core
{
  using PrePost = bool;
  struct PrePostValues
  {
    static constexpr PrePost Pre = true;
    static constexpr PrePost Post = false;
  };

  using OnOff = bool;
  struct OnOffValues
  {
    static constexpr OnOff On = true;
    static constexpr OnOff Off = false;
  };

  enum class ParameterId
  {
    // Globals
    GlobalTempo,
    GlobalVolume,
    GlobalPrelistenVolume,

    GlobalReverbRoomSize,
    GlobalReverbColor,
    GlobalReverbPreDelay,
    GlobalReverbChorus,
    GlobalReverbReturn,
    GlobalReverbOnOff,

    // Per Channel
    ChannelVolume,
    ChannelReverbPrePost,
    ChannelReverbSend,
    ChannelDelayPrePost,
    ChannelDelaySend,
    ChannelOnOff,

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
  };

  using Path = std::filesystem::path;
  using Pattern = std::array<bool, NUM_STEPS>;
  using Float = float;
  using Bool = bool;
  using ChannelNumber = uint32_t;
  using TileNumber = uint32_t;

  struct Address
  {
    std::optional<ChannelNumber> channel;
    std::optional<TileNumber> tile;
  };

  inline bool operator!=(const Address &lhs, const Address &rhs)
  {
    return lhs.channel != rhs.channel || lhs.tile != rhs.tile;
  }

  inline bool operator==(const Address &lhs, const Address &rhs)
  {
    return lhs.channel == rhs.channel && lhs.tile == rhs.tile;
  }

  inline bool operator<(const Address &lhs, const Address &rhs)
  {
    auto lc = lhs.channel.value_or(-1);
    auto lt = lhs.tile.value_or(-1);
    auto rc = rhs.channel.value_or(-1);
    auto rt = rhs.tile.value_or(-1);

    return std::tie(lc, lt) < std::tie(rc, rt);
  }

  using FramePos = int64_t;

  using ParameterValue = std::variant<Bool, Float, Path, Pattern, FramePos, uint8_t>;
}
