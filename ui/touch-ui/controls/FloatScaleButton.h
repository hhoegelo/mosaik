#pragma once

#include <gtkmm/scalebutton.h>
#include "core/Types.h"

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class FloatScaleButton : public Gtk::ScaleButton
  {
   public:
    FloatScaleButton(Core::Api::Interface& core, Core::TileId tileId, Core::ParameterId parameterId, double min,
                     double max, double stepSize, const std::string& img);
  };
}