#pragma once

#include <tools/Format.h>
#include <core/ParameterDescriptor.h>
#include <inttypes.h>
#include <cmath>

namespace Ui
{
  template <Core::ParameterId id> struct ParameterDescriptor : Core::ParameterDescriptor<id>
  {
    static std::string format(typename Core::ParameterDescriptor<id>::Type t)
    {
      return "";
    }
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::GlobalTempo> : Core::ParameterDescriptor<Core::ParameterId::GlobalTempo>
  {
    static std::string format(Type t)
    {
      return Tools::format("%2.1f bpm", t);
    }

    constexpr static auto title = "Tempo";
    constexpr static Type defaultValue = 120;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::GlobalVolume>
      : Core::ParameterDescriptor<Core::ParameterId::GlobalVolume>
  {
    static std::string format(Type t)
    {
      return Tools::format("%2.1f dB", t);
    }

    constexpr static auto title = "Volume";
    constexpr static Type defaultValue = 0;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::Selected> : Core::ParameterDescriptor<Core::ParameterId::Selected>
  {
    static std::string format(Type t)
    {
      return Tools::format("%s", t ? "selected" : "not selected");
    }

    constexpr static auto title = "Selected";
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::Reverse> : Core::ParameterDescriptor<Core::ParameterId::Reverse>
  {
    static std::string format(Type t)
    {
      return Tools::format("%s", t ? "<<<" : ">>>");
    }

    constexpr static auto title = "Reverse";
  };

  template <> struct ParameterDescriptor<Core::ParameterId::Mute> : Core::ParameterDescriptor<Core::ParameterId::Mute>
  {
    static std::string format(Type t)
    {
      return Tools::format("%s", t ? "Muted" : "Unmuted");
    }

    constexpr static auto title = "Mute";
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::Balance> : Core::ParameterDescriptor<Core::ParameterId::Balance>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f %%", 100 * t);
    }

    constexpr static auto title = "Balance";
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<Core::ParameterId::Gain> : Core::ParameterDescriptor<Core::ParameterId::Gain>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f dB", t);
    }

    constexpr static auto title = "Gain";
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<Core::ParameterId::Speed> : Core::ParameterDescriptor<Core::ParameterId::Speed>
  {
    static std::string format(Type t)
    {
      int semitones = std::round(t * 12);
      int octaves = semitones / 12;
      semitones -= octaves * 12;
      return Tools::format("%d oct, %d semi", octaves, std::abs(semitones));
    }

    constexpr static auto title = "Speed";
    constexpr static Type defaultValue = 0;
  };

  namespace detail
  {
    inline std::string formatFramePos(Core::FramePos p)
    {
      if(p == std::numeric_limits<Core::FramePos>::max())
        return "end";
      return Tools::format("%" PRId64, p);
    }
  }
  template <>
  struct ParameterDescriptor<Core::ParameterId::EnvelopeFadeInPos>
      : Core::ParameterDescriptor<Core::ParameterId::EnvelopeFadeInPos>
  {
    static constexpr auto format = detail::formatFramePos;
    constexpr static auto title = "Fade In";
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::EnvelopeFadedInPos>
      : Core::ParameterDescriptor<Core::ParameterId::EnvelopeFadedInPos>
  {
    static constexpr auto format = detail::formatFramePos;
    constexpr static auto title = "Faded In";
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::EnvelopeFadeOutPos>
      : Core::ParameterDescriptor<Core::ParameterId::EnvelopeFadeOutPos>
  {
    static constexpr auto format = detail::formatFramePos;
    constexpr static auto title = "Fade Out";
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::EnvelopeFadedOutPos>
      : Core::ParameterDescriptor<Core::ParameterId::EnvelopeFadedOutPos>
  {
    static constexpr auto format = detail::formatFramePos;
    constexpr static auto title = "Faded Out";
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::TriggerFrame>
      : Core::ParameterDescriptor<Core::ParameterId::TriggerFrame>
  {
    static constexpr auto format = detail::formatFramePos;
    constexpr static auto title = "Hit Point";
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::Shuffle> : Core::ParameterDescriptor<Core::ParameterId::Shuffle>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f %s", 100 * t, unit);
    }

    constexpr static auto title = "Shuffle";
    constexpr static Type defaultValue = 0;
  };

  template <Core::ParameterId P> struct ParameterDescriptionPlayground : Core::ParameterDescriptor<P>
  {
    static std::string format(typename Core::ParameterDescriptor<P>::Type t)
    {
      return Tools::format("%3.1f %%", std::round(100 * t));
    }
    constexpr static Core::ParameterDescriptor<P>::Type defaultValue = 120;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::MainPlayground1>
      : ParameterDescriptionPlayground<Core::ParameterId::MainPlayground1>
  {
    constexpr static auto title = "Playground 1";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::MainPlayground2>
      : ParameterDescriptionPlayground<Core::ParameterId::MainPlayground2>
  {
    constexpr static auto title = "Playground 2";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::MainPlayground3>
      : ParameterDescriptionPlayground<Core::ParameterId::MainPlayground3>
  {
    constexpr static auto title = "Playground 3";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::MainPlayground4>
      : ParameterDescriptionPlayground<Core::ParameterId::MainPlayground4>
  {
    constexpr static auto title = "Playground 4";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::MainPlayground5>
      : ParameterDescriptionPlayground<Core::ParameterId::MainPlayground5>
  {
    constexpr static auto title = "Playground 5";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::MainPlayground6>
      : ParameterDescriptionPlayground<Core::ParameterId::MainPlayground6>
  {
    constexpr static auto title = "Playground 6";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::MainPlayground7>
      : ParameterDescriptionPlayground<Core::ParameterId::MainPlayground7>
  {
    constexpr static auto title = "Playground 7";
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::Playground1>
      : ParameterDescriptionPlayground<Core::ParameterId::Playground1>
  {
    constexpr static auto title = "Playground 1";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::Playground2>
      : ParameterDescriptionPlayground<Core::ParameterId::Playground2>
  {
    constexpr static auto title = "Playground 2";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::Playground3>
      : ParameterDescriptionPlayground<Core::ParameterId::Playground3>
  {
    constexpr static auto title = "Playground 3";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::Playground4>
      : ParameterDescriptionPlayground<Core::ParameterId::Playground4>
  {
    constexpr static auto title = "Playground 4";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::Playground5>
      : ParameterDescriptionPlayground<Core::ParameterId::Playground5>
  {
    constexpr static auto title = "Playground 5";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::Playground6>
      : ParameterDescriptionPlayground<Core::ParameterId::Playground6>
  {
    constexpr static auto title = "Playground 6";
  };
  template <>
  struct ParameterDescriptor<Core::ParameterId::Playground7>
      : ParameterDescriptionPlayground<Core::ParameterId::Playground7>
  {
    constexpr static auto title = "Playground 7";
  };

}
