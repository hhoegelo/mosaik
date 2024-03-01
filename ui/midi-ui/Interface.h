#pragma once

#include <string>
#include <cstdint>
#include <functional>

namespace Ui::Midi
{
  enum class SoftButton
  {
    Left_NorthWest,
    Left_North,
    Left_East,
    Left_SouthEast,
    Left_South,
    Left_SouthWest,
    Left_West,
    Left_Center,

    Right_North,
    Right_NorthEast,
    Right_East,
    Right_SouthEast,
    Right_South,
    Right_SouthWest,
    Right_West,
    Right_Center,
  };

  enum class Knob
  {
    Leftmost,
    Rightmost,
    NorthWest,
    NorthEast,
    SouthWest,
    SouthEast,
    Center
  };

  enum class Color
  {
    Red,
    Blue,
    Green,
    White
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
    }
    return "none";
  }

  class Interface
  {
   public:
    virtual ~Interface();

    virtual void setSoftButtonColor(SoftButton button, Color c) = 0;
    virtual void setStepButtonColor(Step step, Color c) = 0;
    virtual void onSoftButtonEvent(std::function<void(SoftButton, ButtonEvent)> cb) = 0;
    virtual void onStepButtonEvent(std::function<void(Step step, ButtonEvent)> cb) = 0;
    virtual void onKnobEvent(std::function<void(Knob knob, int increments)> cb) = 0;
    virtual void highlightCurrentStep(Step oldStep, Step newStep) = 0;
  };
}