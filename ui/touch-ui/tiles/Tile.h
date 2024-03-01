#pragma once

#include <core/Types.h>
#include <gtkmm/grid.h>
#include "tools/SignalSlot.h"

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
    Widget* buildHasStepsIndicator(Core::Api::Interface& core, Core::TileId tileId);
    Widget* buildPlayIndicator(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Core::TileId tileId);
    Widget* buildVolumeSlider(Core::Api::Interface& core, Core::TileId tileId);
    Widget* buildWaveformDisplay(Core::Api::Interface& core, Core::TileId tileId);
    Widget* buildDurationInSeconds(Core::Api::Interface& core, Core::TileId tileId);
    Widget* buildDurationInSteps(Core::Api::Interface& core, Core::TileId tileId);

    Tools::Signals::Connection m_selectedConnection;
    Tools::Signals::Connection m_patternConnection;
    Tools::Signals::Connection m_reverseConnection;
    Tools::Signals::Connection m_gainConnection;
    Tools::Signals::Connection m_sampleFileConnection;
  };
}