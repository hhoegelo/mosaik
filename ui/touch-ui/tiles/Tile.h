#pragma once

#include <core/Types.h>
#include <gtkmm/grid.h>
#include "tools/SignalSlot.h"
#include "core/api/Interface.h"

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
    Widget* buildWaveformDisplay(Core::Api::Interface& core, Core::TileId tileId);

    Core::Api::Computations m_computations;
  };
}