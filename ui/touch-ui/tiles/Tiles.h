#pragma once

#include "ui/Types.h"
#include "core/Types.h"
#include "tools/ReactiveVar.h"
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
  class Tiles : public Gtk::Grid
  {
   public:
    Tiles(Touch::Interface& touch, Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp,
          Ui::Controller& controller);

   private:
    Tools::DeferredComputations m_computations;
  };

}