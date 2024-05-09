#pragma once

#include "core/api/Interface.h"
#include <gtkmm/drawingarea.h>
#include <gtkmm/styleproperty.h>

namespace Core::Api
{
  class Interface;
}

namespace Dsp::Api::Display
{
  class Interface;
}

namespace Ui::Touch
{
  class WaveformThumb : public Gtk::DrawingArea
  {
   public:
    WaveformThumb(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Core::Address address);
  };
}