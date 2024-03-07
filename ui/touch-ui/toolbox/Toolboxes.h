#pragma once

#include <gtkmm/notebook.h>
#include <optional>
#include <gtkmm/levelbar.h>
#include "core/Types.h"
#include "tools/SignalSlot.h"

namespace Core::Api
{
  class Interface;
  class Computation;
}

namespace Ui
{
  class SharedState;

  namespace Touch
  {
    class Toolboxes : public Gtk::Notebook
    {
     public:
      explicit Toolboxes(SharedState &sharedUiState, Core::Api::Interface &core);

     private:
      Widget *buildLevel(Core::TileId tile, Core::ParameterId id, double min, double max);
      Widget *buildGlobals();
      Widget *buildTileToolbox();

      Core::Api::Interface &m_core;
      Glib::RefPtr<Gio::File> m_lastSelectedFolder;
      std::vector<Tools::Signals::Connection> m_connections;

      std::unique_ptr<Core::Api::Computation> m_tilePageGain;
      void showTileGain(Gtk::LevelBar *pBar);
    };
  }
}