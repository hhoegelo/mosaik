#pragma once

#include "ui/Types.h"
#include <gtkmm/grid.h>

namespace Core::Api
{
  class Interface;
}

namespace Dsp::Api::Display
{
  class Interface;
}

namespace Ui
{
  class Controller;

  namespace Touch
  {
    class Interface;
  }
}

namespace Ui::Touch
{
  class Mixer : public Gtk::Grid
  {
   public:
    Mixer(Touch::Interface& touch, Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp,
          Ui::Controller& controller);
  };

}