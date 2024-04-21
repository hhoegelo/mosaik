#pragma once

#include "ui/Types.h"
#include <ui/touch-ui/sections/SectionWrapper.h>
#include <gtkmm/grid.h>

namespace Ui::Touch
{
  class Main : public SectionWrapper<Section::Main, Gtk::Grid>
  {
   public:
    Main(Touch::Interface& touch);
  };

}