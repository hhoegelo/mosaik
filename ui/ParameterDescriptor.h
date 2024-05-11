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
    constexpr static float acceleration = 5.f;
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
    constexpr static float acceleration = 5.f;
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
    constexpr static float acceleration = 5.f;
  };

  template <> struct ParameterDescriptor<Core::ParameterId::Gain> : Core::ParameterDescriptor<Core::ParameterId::Gain>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f dB", t);
    }

    constexpr static auto title = "Gain";
    constexpr static float acceleration = 5.f;
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
    constexpr static float acceleration = 10.f;
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
    constexpr static float acceleration = 10.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::EnvelopeFadedInPos>
      : Core::ParameterDescriptor<Core::ParameterId::EnvelopeFadedInPos>
  {
    static constexpr auto format = detail::formatFramePos;
    constexpr static auto title = "Faded In";
    constexpr static float acceleration = 10.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::EnvelopeFadeOutPos>
      : Core::ParameterDescriptor<Core::ParameterId::EnvelopeFadeOutPos>
  {
    static constexpr auto format = detail::formatFramePos;
    constexpr static auto title = "Fade Out";
    constexpr static float acceleration = 10.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::EnvelopeFadedOutPos>
      : Core::ParameterDescriptor<Core::ParameterId::EnvelopeFadedOutPos>
  {
    static constexpr auto format = detail::formatFramePos;
    constexpr static auto title = "Faded Out";
    constexpr static float acceleration = 10.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::TriggerFrame>
      : Core::ParameterDescriptor<Core::ParameterId::TriggerFrame>
  {
    static constexpr auto format = detail::formatFramePos;
    constexpr static auto title = "Hit Point";
    constexpr static float acceleration = 10.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::Shuffle> : Core::ParameterDescriptor<Core::ParameterId::Shuffle>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f %%", 100 * t);
    }

    constexpr static auto title = "Shuffle";
    constexpr static float acceleration = 5.f;
  };

  template <Core::ParameterId P> struct ParameterDescriptionPlayground : Core::ParameterDescriptor<P>
  {
    static std::string format(typename Core::ParameterDescriptor<P>::Type t)
    {
      return Tools::format("%3.1f %%", std::round(100 * t));
    }

    constexpr static float acceleration = 10.f;
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

  template <>
  struct ParameterDescriptor<Core::ParameterId::ChannelOnOff>
      : Core::ParameterDescriptor<Core::ParameterId::ChannelOnOff>
  {
    static std::string format(Type t)
    {
      return t == Core::OnOffValues::On ? "On" : "Off";
    }

    constexpr static auto title = "On / Off";
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::ChannelVolume>
      : Core::ParameterDescriptor<Core::ParameterId::ChannelVolume>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f dB", t);
    }

    constexpr static auto title = "Volume";
    constexpr static float acceleration = 5.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::ChannelDelayPrePost>
      : Core::ParameterDescriptor<Core::ParameterId::ChannelDelayPrePost>
  {
    static std::string format(Type t)
    {
      return t == Core::PrePostValues::Pre ? "Pre" : "Post";
    }

    constexpr static auto title = "Delay Pre/Post";
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::ChannelReverbPrePost>
      : Core::ParameterDescriptor<Core::ParameterId::ChannelReverbPrePost>
  {
    static std::string format(Type t)
    {
      return t == Core::PrePostValues::Pre ? "Pre" : "Post";
    }

    constexpr static auto title = "Reverb Pre/Post";
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::ChannelDelaySend>
      : Core::ParameterDescriptor<Core::ParameterId::ChannelDelaySend>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f dB", t);
    }

    constexpr static auto title = "Delay Send";
    constexpr static float acceleration = 5.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::ChannelReverbSend>
      : Core::ParameterDescriptor<Core::ParameterId::ChannelReverbSend>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f dB", t);
    }

    constexpr static auto title = "Reverb Send";
    constexpr static float acceleration = 5.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::GlobalReverbRoomSize>
      : Core::ParameterDescriptor<Core::ParameterId::GlobalReverbRoomSize>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f %%", t * 100);
    }

    constexpr static auto title = "Room Size";
    constexpr static float acceleration = 5.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::GlobalReverbColor>
      : Core::ParameterDescriptor<Core::ParameterId::GlobalReverbColor>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f %%", t * 100);
    }

    constexpr static auto title = "Color";
    constexpr static float acceleration = 5.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::GlobalReverbPreDelay>
      : Core::ParameterDescriptor<Core::ParameterId::GlobalReverbPreDelay>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f %%", t * 100);
    }

    constexpr static auto title = "Pre Delay";
    constexpr static float acceleration = 5.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::GlobalReverbChorus>
      : Core::ParameterDescriptor<Core::ParameterId::GlobalReverbChorus>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f %%", t * 100);
    }

    constexpr static auto title = "Chorus";
    constexpr static float acceleration = 5.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::GlobalReverbReturn>
      : Core::ParameterDescriptor<Core::ParameterId::GlobalReverbReturn>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f db", t);
    }

    constexpr static auto title = "Return";
    constexpr static float acceleration = 5.f;
  };

  template <>
  struct ParameterDescriptor<Core::ParameterId::GlobalReverbOnOff>
      : Core::ParameterDescriptor<Core::ParameterId::GlobalReverbOnOff>
  {
    static std::string format(Type t)
    {
      return t == Core::OnOffValues::On ? "On" : "Off";
    }

    constexpr static auto title = "On/Off";
  };
}
