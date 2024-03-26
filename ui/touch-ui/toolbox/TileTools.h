#pragma once

#include <core/api/Interface.h>
#include <tools/ReactiveVar.h>
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
    void updateTileGain(Gtk::Label *level);
    void updateTileSpeed(Gtk::Label *level);
    void updateTileBalance(Gtk::Label *level);

    Core::Api::Interface &m_core;

    Gtk::FileChooserWidget *m_fileBrowser;
    std::optional<std::string> m_lastSelectedFolder;
    Tools::DeferredComputations m_computations;
  };
}