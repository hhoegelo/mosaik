#pragma once

#include "tools/SignalSlot.h"
#include "core/api/Interface.h"

#include <gtkmm/box.h>
#include <optional>

namespace Core::Api
{
  class Interface;
}

namespace Gtk
{
  class LevelBar;
  class FileChooserWidget;
}

namespace Ui::Touch
{
  class TileTools : public Gtk::Box
  {
   public:
    explicit TileTools(Core::Api::Interface &core);

    void onToolShown();

   private:
    void updateTileGain(Core::Api::Computation *c, Gtk::LevelBar *level);
    void updateTileSpeed(Core::Api::Computation *c, Gtk::LevelBar *level);
    void updateTileBalance(Core::Api::Computation *c, Gtk::LevelBar *level);

    Core::Api::Interface &m_core;

    Gtk::FileChooserWidget *m_fileBrowser;
    std::optional<std::string> m_lastSelectedFolder;
    Core::Api::Computations m_computations;
  };
}