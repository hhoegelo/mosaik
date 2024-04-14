#pragma once

#include <tools/Format.h>
#include <dsp/Types.h>
#include <filesystem>
#include <variant>
#include <optional>
#include <vector>
//#include <format>

namespace Core
{
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
  };

  using Path = std::filesystem::path;
  using Pattern = std::array<bool, NUM_STEPS>;
  using Float = float;
  using Bool = bool;
  using TileId = std::optional<uint32_t>;
  using FramePos = int64_t;

  using ParameterValue = std::variant<Bool, Float, Path, Pattern, FramePos>;

  template <ParameterId id> struct ParameterDescription;

  template <> struct ParameterDescription<ParameterId::GlobalTempo>
  {
    constexpr static ParameterId id = ParameterId::GlobalTempo;
    constexpr static auto name = "tempo";
    constexpr static const char* title[] = { "Tempo", "Tmp" };
    using Type = Float;
    constexpr static auto min = 20.0f;
    constexpr static auto max = 240.0f;
    constexpr static auto coarse = 1.0f;
    constexpr static auto fine = 0.1f;
    constexpr static auto unit = "bpm";

    static std::string format(Type t)
    {
      return Tools::format("%3.1f %s", t, unit);
    }
  };

  template <> struct ParameterDescription<ParameterId::GlobalVolume>
  {
    constexpr static ParameterId id = ParameterId::GlobalVolume;
    constexpr static auto name = "volume";
    constexpr static const char* title[] = { "Master Volume", "Volume", "Vol" };
    using Type = Float;
    constexpr static auto min = -72.0f;
    constexpr static auto max = 0.0f;
    constexpr static auto coarse = 0.5f;
    constexpr static auto fine = 0.1f;
    constexpr static auto unit = "dB";

    static std::string format(Type t)
    {
      return Tools::format("%2.1f %s", t, unit);
    }
  };

  template <> struct ParameterDescription<ParameterId::Selected>
  {
    constexpr static ParameterId id = ParameterId::Selected;
    using Type = Bool;
    constexpr static auto name = "selected";
    constexpr static const char* title[] = { "Selected", "Sel" };

    static std::string format(Type t)
    {
      return Tools::format("%s", t ? "yes" : "no");
    }
  };

  template <> struct ParameterDescription<ParameterId::Reverse>
  {
    constexpr static ParameterId id = ParameterId::Reverse;
    using Type = Bool;
    constexpr static auto name = "reverse";
    constexpr static const char* title[] = { "Reverse", "Rev" };

    static std::string format(Type t)
    {
      return Tools::format("%s", t ? "<<<" : ">>>");
    }
  };

  template <> struct ParameterDescription<ParameterId::SampleFile>
  {
    constexpr static ParameterId id = ParameterId::SampleFile;
    using Type = Path;
    constexpr static auto name = "sample";
    constexpr static const char* title[] = { "Sample" };

    static std::string format(const Type& t)
    {
      return t.string();
    }
  };

  template <> struct ParameterDescription<ParameterId::Pattern>
  {
    constexpr static ParameterId id = ParameterId::Pattern;
    using Type = Pattern;
    constexpr static auto name = "pattern";
    constexpr static const char* title[] = { "Pattern" };

    static std::string format(const Type& t)
    {
      return "n/a";
    }
  };

  template <> struct ParameterDescription<ParameterId::Mute>
  {
    constexpr static ParameterId id = ParameterId::Mute;
    using Type = Bool;
    constexpr static auto name = "mute";
    constexpr static const char* title[] = { "Mute" };

    static std::string format(const Type& t)
    {
      return Tools::format("%s", t ? "muted" : "unmuted");
    }
  };

  template <> struct ParameterDescription<ParameterId::Balance>
  {
    constexpr static ParameterId id = ParameterId::Balance;
    constexpr static auto name = "balance";
    constexpr static const char* title[] = { "Balance", "Bal" };
    using Type = Float;
    constexpr static auto min = -1.0f;
    constexpr static auto max = 1.0f;
    constexpr static auto coarse = 0.01f;
    constexpr static auto fine = 0.005f;
    constexpr static auto unit = "%";

    static std::string format(Type t)
    {
      return Tools::format("%3.2f %s", 100 * t, unit);
    }
  };

  template <> struct ParameterDescription<ParameterId::Gain>
  {
    constexpr static ParameterId id = ParameterId::Gain;
    constexpr static auto name = "gain";
    constexpr static const char* title[] = { "Gain" };
    using Type = Float;
    constexpr static auto min = -48.0f;
    constexpr static auto max = 6.0f;
    constexpr static auto coarse = 0.1f;
    constexpr static auto fine = 0.01f;
    constexpr static auto unit = "dB";

    static std::string format(Type t)
    {
      return Tools::format("%3.2f %s", t, unit);
    }
  };

  template <> struct ParameterDescription<ParameterId::Speed>
  {
    constexpr static ParameterId id = ParameterId::Speed;
    constexpr static auto name = "speed";
    constexpr static const char* title[] = { "Speed", "Spd" };
    using Type = Float;
    constexpr static auto min = 50.0f;
    constexpr static auto max = 200.0f;
    constexpr static auto coarse = 1.f;
    constexpr static auto fine = 0.1f;
    constexpr static auto unit = "%";

    static std::string format(Type t)
    {
      return Tools::format("%3.2f %s", t, unit);
    }
  };

  template <> struct ParameterDescription<ParameterId::EnvelopeFadeInPos>
  {
    constexpr static ParameterId id = ParameterId::EnvelopeFadeInPos;
    constexpr static auto name = "envelopeFadeInPos";
    constexpr static const char* title[] = { "FadeIn Position", "FadeIn Pos", "FadeIn" };
    using Type = FramePos;

    static std::string format(Type t)
    {
      return Tools::format("%zu", t);
    }
  };

  template <> struct ParameterDescription<ParameterId::EnvelopeFadedInPos>
  {
    constexpr static ParameterId id = ParameterId::EnvelopeFadedInPos;
    constexpr static auto name = "envelopeFadedInPos";
    constexpr static const char* title[] = { "FadedIn Position", "FadedIn Pos", "FadedIn" };
    using Type = FramePos;

    static std::string format(Type t)
    {
      return Tools::format("%zu", t);
    }
  };

  template <> struct ParameterDescription<ParameterId::EnvelopeFadeOutPos>
  {
    constexpr static ParameterId id = ParameterId::EnvelopeFadeOutPos;
    constexpr static auto name = "envelopeFadeOutPos";
    constexpr static const char* title[] = { "FadeOut Position", "FadeOut Pos", "FadeOut" };
    using Type = FramePos;

    static std::string format(Type t)
    {
      return Tools::format("%zu", t);
    }
  };

  template <> struct ParameterDescription<ParameterId::EnvelopeFadedOutPos>
  {
    constexpr static ParameterId id = ParameterId::EnvelopeFadedOutPos;
    constexpr static auto name = "envelopeFadedOutPos";
    constexpr static const char* title[] = { "FadedOut Position", "FadedOut Pos", "FadedOut" };
    using Type = FramePos;

    static std::string format(Type t)
    {
      return Tools::format("%zu", t);
    }
  };

  template <> struct ParameterDescription<ParameterId::TriggerFrame>
  {
    constexpr static ParameterId id = ParameterId::TriggerFrame;
    constexpr static auto name = "triggerFrame";
    constexpr static const char* title[] = { "Hit Point", "Hit" };
    using Type = FramePos;

    static std::string format(Type t)
    {
      return Tools::format("%zu", t);
    }
  };

  template <> struct ParameterDescription<ParameterId::Shuffle>
  {
    constexpr static ParameterId id = ParameterId::Shuffle;
    constexpr static auto name = "shuffle";
    constexpr static const char* title[] = { "Shuffle", "Shuf" };
    using Type = Float;
    constexpr static auto min = -1.0f;
    constexpr static auto max = 1.0f;
    constexpr static auto coarse = 0.01f;
    constexpr static auto fine = 0.005f;
    constexpr static auto unit = "%";

    static std::string format(Type t)
    {
      return Tools::format("%3.2f %s", 100 * t, unit);
    }
  };

  template <ParameterId... ids> struct Parameters
  {
    using Descriptors = std::tuple<ParameterDescription<ids>...>;

    static bool contains(ParameterId id)
    {
      return ((ids == id) || ...);
    }
  };

  using GlobalParameters = Parameters<ParameterId::GlobalTempo, ParameterId::GlobalVolume>;

  using TileParameters
      = Parameters<ParameterId::Selected, ParameterId::SampleFile, ParameterId::Reverse, ParameterId::Pattern,
                   ParameterId::Balance, ParameterId::Gain, ParameterId::Mute, ParameterId::Speed,
                   ParameterId::EnvelopeFadeInPos, ParameterId::EnvelopeFadedInPos, ParameterId::EnvelopeFadeOutPos,
                   ParameterId::EnvelopeFadedOutPos, ParameterId::TriggerFrame, ParameterId::Shuffle>;
}
