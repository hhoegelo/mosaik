#pragma once

#include "ui/touch-ui/Interface.h"
#include "tools/ReactiveVar.h"
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
  class FileBrowser;

  class TileTools : public Gtk::Box
  {
   public:
    explicit TileTools(Core::Api::Interface &core);

    FileBrowserInterface &getFileBrowser() const;

   private:
    Core::Api::Interface &m_core;
    FileBrowser *m_fileBrowser;
    Tools::DeferredComputations m_computations;
  };
}