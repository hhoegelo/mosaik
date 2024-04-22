#pragma once

#include "tools/ReactiveVar.h"

#include "core/Types.h"
#include "core/api/Interface.h"

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
  class Tile : public Gtk::Grid
  {
   public:
    Tile(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Core::TileId tileId);

   private:
    void runLevelMeterTimer(Dsp::Api::Display::Interface& dsp, Core::TileId tileId, Gtk::Label* reverse);

    Tools::DeferredComputations m_computations;
  };
}