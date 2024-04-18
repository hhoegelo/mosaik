#pragma once

#include <ui/touch-ui/Interface.h>
#include <tools/ReactiveVar.h>
#include <gtkmm/box.h>

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
  class TileTools : public Gtk::Box, public FileBrowserInterface
  {
   public:
    explicit TileTools(Core::Api::Interface &core);

    void up() override;
    void down() override;
    void inc() override;
    void dec() override;
    void load() override;
    void prelisten() override;

   private:
    void navigate(const std::function<void(Gtk::TreePath &)> &cb);

    Core::Api::Interface &m_core;

    Gtk::FileChooserWidget *m_fileBrowser;
    Tools::DeferredComputations m_computations;
  };
}