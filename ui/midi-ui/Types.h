#pragma once

#include <cstdint>
#include <string>

namespace Ui::Midi
{
  enum class SoftButton
  {
    // have to agree on numbers here for the translation from midi events to work properly
    Left_NorthWest,
    Left_North,
    Left_East,
    Left_South = 70,
    Left_SouthEast = 71,
    Left_SouthWest,
    Left_West,
    Left_Center,

    Right_North,
    Right_NorthEast,
    Right_East,
    Right_SouthEast,
    Right_South,
    Right_SouthWest = 101,
    Right_West = 102,
    Right_Center,

    // Knobs are Buttons, too
    Leftmost = 88,
    Rightmost = 94,
    NorthWest = 89,
    NorthEast = 93,
    SouthWest = 90,
    SouthEast = 92,
    Center = 91
  };

  enum class Knob
  {
    Leftmost = 0,
    Rightmost = 6,
    NorthWest = 1,
    NorthEast = 5,
    SouthWest = 2,
    SouthEast = 4,
    Center = 3
  };

  enum class Color
  {
    None = 0,
    Red = 1,
    Green = 2,
    Blue = 3,
    White = 4
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
    }
    return "none";
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

      case SoftButton::Leftmost:
        return "LeftMost";
      case SoftButton::Rightmost:
        return "RightMost";
      case SoftButton::NorthWest:
        return "NorthWest";
      case SoftButton::NorthEast:
        return "NorthEast";
      case SoftButton::SouthWest:
        return "SouthWest";
      case SoftButton::SouthEast:
        return "SouthEast";
      case SoftButton::Center:
        return "Center";
    }
    return "none";
  }
}