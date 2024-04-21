#pragma once

#include "ui/Types.h"
#include <ui/touch-ui/sections/SectionWrapper.h>
#include <gtkmm/grid.h>

namespace Ui::Touch
{
  class Mixer : public SectionWrapper<Section::Mixer, Gtk::Grid>
  {
   public:
    Mixer(Touch::Interface& touch);
  };

}