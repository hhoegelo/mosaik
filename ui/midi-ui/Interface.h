#pragma once

#include <ui/midi-ui/Types.h>
#include <functional>

namespace Ui::Midi
{
  class Interface
  {
   public:
    virtual ~Interface();

    virtual void setLed(Led l, Color c) = 0;
  };
}