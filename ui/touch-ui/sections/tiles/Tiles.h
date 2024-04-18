#pragma once

#include "ui/Types.h"
#include "core/Types.h"
#include <ui/touch-ui/sections/SectionWrapper.h>
#include <gtkmm/grid.h>

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
  class Tiles : public SectionWrapper<Section::Tiles, Gtk::Grid>
  {
   public:
    Tiles(Touch::Interface& touch, Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp);
  };

}