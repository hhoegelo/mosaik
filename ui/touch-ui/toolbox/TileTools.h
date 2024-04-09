#pragma once

#include <core/api/Interface.h>
#include <ui/touch-ui/Interface.h>
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
  class TreePath;
}

namespace Ui::Touch
{
  class TileTools : public Gtk::Box, public Interface::FileBrowser
  {
   public:
    explicit TileTools(Core::Api::Interface &core);

    void up() override;
    void down() override;
    void inc() override;
    void dec() override;
    void load() override;

   private:
    void updateTileGain(Gtk::Label *level);
    void updateTileSpeed(Gtk::Label *level);
    void updateTileBalance(Gtk::Label *level);

    void navigate(const std::function<void(Gtk::TreePath &)> &cb);

    Core::Api::Interface &m_core;

    Gtk::FileChooserWidget *m_fileBrowser;
    std::optional<std::string> m_lastSelectedFolder;
    Tools::DeferredComputations m_computations;
  };
}