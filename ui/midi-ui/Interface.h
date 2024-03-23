#pragma once

#include <ui/midi-ui/Types.h>
#include <functional>

namespace Ui::Midi
{
  class Interface
  {
   public:
    virtual ~Interface();

    virtual void setSoftButtonColor(SoftButton button, Color c) = 0;
    virtual void setStepButtonColor(Step step, Color c) = 0;
    //    virtual void onSoftButtonEvent(std::function<void(SoftButton, ButtonEvent)> cb) = 0;
    //   virtual void onStepButtonEvent(std::function<void(Step step, ButtonEvent)> cb) = 0;
    //  virtual void onKnobEvent(std::function<void(Knob knob, int increments)> cb) = 0;
    virtual void highlightCurrentStep(Step oldStep, Step newStep) = 0;
  };
}