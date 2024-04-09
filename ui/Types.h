#pragma once

#include <cstdint>
#include <string>

namespace Ui
{
  using Row = uint8_t;
  using Col = uint8_t;

  enum class Toolboxes
  {
    Global,
    Tile,
    Waveform
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

}
