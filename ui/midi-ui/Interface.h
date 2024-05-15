#pragma once

#include <ui/Types.h>
#include <functional>

namespace Ui::Midi
{
  class Interface
  {
   public:
    virtual ~Interface();

    virtual void setLed(Knob k, Color c) = 0;
    virtual void setLed(SoftButton s, Color c) = 0;
    virtual void setLed(SoftButton s, uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual void setLed(Step s, Color c) = 0;
  };
}