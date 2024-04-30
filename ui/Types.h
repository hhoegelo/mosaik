#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <array>
#include <core/Types.h>

template <class... Ts> struct overloaded : Ts...
{
  using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace Ui
{
  using Row = uint8_t;
  using Col = uint8_t;

  enum class Toolbox
  {
    Global,
    Tile,
    Waveform,
    Steps,
    Playground,
    MainPlayground
  };

  enum class Section
  {
    Tiles,
    Toolboxes,
    Mixer,
    Main
  };

  enum class SoftButton
  {
    // Left hand side buttons
    Left_NorthWest = 64,
    Left_North = 65,
    Left_West = 66,
    Left_Center = 67,
    Left_East = 68,
    Left_SouthWest = 69,
    Left_South = 70,
    Left_SouthEast = 71,

    // Right hand side buttons
    Right_North = 96,
    Right_NorthEast = 97,
    Right_West = 98,
    Right_Center = 99,
    Right_East = 100,
    Right_SouthWest = 101,
    Right_South = 102,
    Right_SouthEast = 103,

    // Knobs are Buttons, too
    Center_Leftmost = 88,
    Center_NorthWest = 89,
    Center_SouthWest = 90,
    Center_Center = 91,
    Center_SouthEast = 92,
    Center_NorthEast = 93,
    Center_Rightmost = 94,
  };

  enum class Knob
  {
    Leftmost = 0,
    NorthWest = 1,
    SouthWest = 2,
    Center = 3,
    SouthEast = 4,
    NorthEast = 5,
    Rightmost = 6,
  };

  enum class Led
  {
    // Steps
    Step_0 = 0,
    Step_63 = 63,

    // Soft Buttons
    Left_NorthWest = 64,
    Left_North = 65,
    Left_West = 66,
    Left_Center = 67,
    Left_East = 68,
    Left_SouthWest = 69,
    Left_South = 70,
    Left_SouthEast = 71,

    Center_Leftmost_North = 72,
    Center_Leftmost_East = 73,
    Center_Leftmost_South = 74,
    Center_Leftmost_West = 75,

    Center_NorthWest_North = 76,
    Center_NorthWest_East = 77,
    Center_NorthWest_South = 78,
    Center_NorthWest_West = 79,

    Center_SouthWest_North = 80,
    Center_SouthWest_East = 81,
    Center_SouthWest_South = 82,
    Center_SouthWest_West = 83,

    Center_Center_North = 84,
    Center_Center_East = 85,
    Center_Center_South = 86,
    Center_Center_West = 87,

    Center_SouthEast_North = 88,
    Center_SouthEast_East = 89,
    Center_SouthEast_South = 90,
    Center_SouthEast_West = 91,

    Center_NorthEast_North = 92,
    Center_NorthEast_East = 93,
    Center_NorthEast_South = 94,
    Center_NorthEast_West = 95,

    Center_Rightmost_North = 96,
    Center_Rightmost_East = 97,
    Center_Rightmost_South = 98,
    Center_Rightmost_West = 99,

    Right_North = 100,
    Right_NorthEast = 101,
    Right_West = 102,
    Right_Center = 103,
    Right_East = 104,
    Right_SouthWest = 105,
    Right_South = 106,
    Right_SouthEast = 107,

    NUM_LEDS = 108
  };

  enum class Color
  {
    None = -1,
    Red = 0,
    Blue = 1,
    Green = 2,
    White = 3,
    Purple = 4,
    Off = 5,
  };

  enum class ButtonEvent
  {
    Press,
    Release
  };

  using Step = uint8_t;

  static inline std::string getColorName(Color c)
  {
    switch(c)
    {
      case Color::Red:
        return "red";
      case Color::Blue:
        return "blue";
      case Color::Green:
        return "green";
      case Color::White:
        return "white";
      case Color::Purple:
        return "purple";
      case Color::Off:
        return "off";
    }
    return "off";
  }

  static inline std::string getKnobName(Knob c)
  {
    switch(c)
    {
      case Knob::Leftmost:
        return "LeftMost";
      case Knob::Rightmost:
        return "RightMost";
      case Knob::NorthWest:
        return "NorthWest";
      case Knob::NorthEast:
        return "NorthEast";
      case Knob::SouthWest:
        return "SouthWest";
      case Knob::SouthEast:
        return "SouthEast";
      case Knob::Center:
        return "Center";
    }
    return "none";
  }

  static inline std::string getSoftButtonName(SoftButton c)
  {
    switch(c)
    {
      case SoftButton::Left_NorthWest:
        return "Left_NorthWest";
      case SoftButton::Left_North:
        return "Left_North";
      case SoftButton::Left_East:
        return "Left_East";
      case SoftButton::Left_SouthEast:
        return "Left_SouthEast";
      case SoftButton::Left_South:
        return "Left_South";
      case SoftButton::Left_SouthWest:
        return "Left_SouthWest";
      case SoftButton::Left_West:
        return "Left_West";
      case SoftButton::Left_Center:
        return "Left_Center";

      case SoftButton::Right_North:
        return "Right_North";
      case SoftButton::Right_NorthEast:
        return "Right_NorthEast";
      case SoftButton::Right_East:
        return "Right_East";
      case SoftButton::Right_SouthEast:
        return "Right_SouthEast";
      case SoftButton::Right_South:
        return "Right_South";
      case SoftButton::Right_SouthWest:
        return "Right_SouthWest";
      case SoftButton::Right_West:
        return "Right_West";
      case SoftButton::Right_Center:
        return "Right_Center";

      case SoftButton::Center_Leftmost:
        return "Center_LeftMost";
      case SoftButton::Center_Rightmost:
        return "Center_RightMost";
      case SoftButton::Center_NorthWest:
        return "Center_NorthWest";
      case SoftButton::Center_NorthEast:
        return "Center_NorthEast";
      case SoftButton::Center_SouthWest:
        return "Center_SouthWest";
      case SoftButton::Center_SouthEast:
        return "Center_SouthEast";
      case SoftButton::Center_Center:
        return "Center_Center";
    }
    return "none";
  }

  template <Toolbox t> struct ToolboxDefinition;

  template <typename... Entries> struct List
  {
    template <typename T> void forEach(const T &cb)
    {
      (cb(Entries {}), ...);
    }
  };

  template <> struct ToolboxDefinition<Toolbox::Global>
  {
    constexpr static auto title = "Global";

    using Minimized = List<Core::ParameterId::GlobalVolume, Core::ParameterId::GlobalTempo>;

    static constexpr std::tuple maximized = {
      std::make_tuple(Core::ParameterId::GlobalVolume, Color::Green, Knob::Center),
      std::make_tuple(Core::ParameterId::GlobalTempo, Color::Blue, Knob::Leftmost),
    };
  };

  template <> struct ToolboxDefinition<Toolbox::MainPlayground>
  {
    constexpr static auto title = "Main Playground";

    static constexpr Core::ParameterId minimized[] {};

    static constexpr std::tuple maximized = {};
  };

  template <> struct ToolboxDefinition<Toolbox::Playground>
  {
    constexpr static auto title = "Playground";

    static constexpr Core::ParameterId minimized[] {};

    static constexpr std::tuple maximized = {};
  };

  template <> struct ToolboxDefinition<Toolbox::Steps>
  {
    constexpr static auto title = "Step Wizard";

    static constexpr Core::ParameterId minimized[] {};

    static constexpr std::tuple maximized = {};
  };

  template <> struct ToolboxDefinition<Toolbox::Tile>
  {
    constexpr static auto title = "Tile";

    static constexpr Core::ParameterId minimized[] {};

    static constexpr std::tuple maximized = {};
  };

  template <> struct ToolboxDefinition<Toolbox::Waveform>
  {
    constexpr static auto title = "Waveform";

    static constexpr Core::ParameterId minimized[] {};

    static constexpr std::tuple maximized = {};
  };

  template <Core::ParameterId id> struct ParameterDescription : Core::ParameterDescription<id>
  {
    static std::string format(typename Core::ParameterDescription<id>::Type t)
    {
      return "";
    }
  };

  template <>
  struct ParameterDescription<Core::ParameterId::GlobalTempo>
      : Core::ParameterDescription<Core::ParameterId::GlobalTempo>
  {
    static std::string format(Type t)
    {
      return Tools::format("%2.1f bpm", t);
    }

    constexpr static auto title = "Tempo";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::GlobalVolume>
      : Core::ParameterDescription<Core::ParameterId::GlobalVolume>
  {
    static std::string format(Type t)
    {
      return Tools::format("%2.1f dB", t);
    }

    constexpr static auto title = "Volume";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::Selected> : Core::ParameterDescription<Core::ParameterId::Selected>
  {
    static std::string format(Type t)
    {
      return Tools::format("%s", t ? "selected" : "not selected");
    }

    constexpr static auto title = "Selected";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::Reverse> : Core::ParameterDescription<Core::ParameterId::Reverse>
  {
    static std::string format(Type t)
    {
      return Tools::format("%s", t ? "<<<" : ">>>");
    }

    constexpr static auto title = "Reverse";
  };

  template <> struct ParameterDescription<Core::ParameterId::Mute> : Core::ParameterDescription<Core::ParameterId::Mute>
  {
    static std::string format(Type t)
    {
      return Tools::format("%s", t ? "Muted" : "Unmuted");
    }

    constexpr static auto title = "Mute";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::Balance> : Core::ParameterDescription<Core::ParameterId::Balance>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f %%", 100 * t);
    }

    constexpr static auto title = "Balance";
  };

  template <> struct ParameterDescription<Core::ParameterId::Gain> : Core::ParameterDescription<Core::ParameterId::Gain>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f dB", t);
    }

    constexpr static auto title = "Gain";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::Speed> : Core::ParameterDescription<Core::ParameterId::Speed>
  {
    static std::string format(Type t)
    {
      int semitones = std::round(t * 12);
      int octaves = semitones / 12;
      semitones -= octaves * 12;
      return Tools::format("%d oct, %d semi", octaves, std::abs(semitones));
    }

    constexpr static auto title = "Speed";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::EnvelopeFadeInPos>
      : Core::ParameterDescription<Core::ParameterId::EnvelopeFadeInPos>
  {
    static std::string format(Type t)
    {
      return Tools::format("%zu", t);
    }

    constexpr static auto title = "Fade In";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::EnvelopeFadedInPos>
      : Core::ParameterDescription<Core::ParameterId::EnvelopeFadedInPos>
  {
    static std::string format(Type t)
    {
      return Tools::format("%zu", t);
    }

    constexpr static auto title = "Faded In";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::EnvelopeFadeOutPos>
      : Core::ParameterDescription<Core::ParameterId::EnvelopeFadeOutPos>
  {
    static std::string format(Type t)
    {
      return Tools::format("%zu", t);
    }

    constexpr static auto title = "Fade Out";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::EnvelopeFadedOutPos>
      : Core::ParameterDescription<Core::ParameterId::EnvelopeFadedOutPos>
  {
    static std::string format(Type t)
    {
      return Tools::format("%zu", t);
    }

    constexpr static auto title = "Faded Out";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::TriggerFrame>
      : Core::ParameterDescription<Core::ParameterId::TriggerFrame>
  {
    static std::string format(Type t)
    {
      return Tools::format("%zu", t);
    }

    constexpr static auto title = "Hit Point";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::Shuffle> : Core::ParameterDescription<Core::ParameterId::Shuffle>
  {
    static std::string format(Type t)
    {
      return Tools::format("%3.2f %s", 100 * t, unit);
    }

    constexpr static auto title = "Shuffle";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::WizardMode> : Core::ParameterDescription<Core::ParameterId::WizardMode>
  {
    static std::string format(Type t)
    {
      switch(static_cast<Core::WizardMode>(t))
      {
        case Core::Or:
          return "Or";
        case Core::And:
          return "And";
        case Core::Replace:
          return "Replace";
        case Core::Not:
          return "Not";
      }
      return "n/a";
    }

    constexpr static auto title = "Wizard Mode";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::WizardRotate>
      : Core::ParameterDescription<Core::ParameterId::WizardRotate>
  {
    static std::string format(Type t)
    {
      return Tools::format("%2.0f steps", std::round(t));
    }

    constexpr static auto title = "Rotate";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::WizardOns> : Core::ParameterDescription<Core::ParameterId::WizardOns>
  {
    static std::string format(Type t)
    {
      return Tools::format("%2.0f steps", std::round(t));
    }

    constexpr static auto title = "On's";
  };

  template <>
  struct ParameterDescription<Core::ParameterId::WizardOffs> : Core::ParameterDescription<Core::ParameterId::WizardOffs>
  {
    static std::string format(Type t)
    {
      return Tools::format("%2.0f steps", std::round(t));
    }

    constexpr static auto title = "Off's";
  };

  template <Core::ParameterId P> struct ParameterDescriptionPlayground : Core::ParameterDescription<P>
  {
    static std::string format(typename Core::ParameterDescription<P>::Type t)
    {
      return Tools::format("%3.1f %%", std::round(100 * t));
    }

    constexpr static auto title = "Playground";
  };

}
