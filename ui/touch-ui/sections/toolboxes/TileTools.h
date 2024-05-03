#pragma once

#include <ui/Types.h>
#include "GenericToolbox.h"
#include <ui/touch-ui/Interface.h>

namespace Gtk
{
  class FileChooserWidget;
}

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{

  class TileTools : public GenericMaximized<Ui::Toolbox::Tile>, public FileBrowserInterface
  {
   public:
    TileTools(Core::Api::Interface &core, Ui::Controller &controller);

    void up() override;
    void down() override;
    void inc() override;
    void dec() override;
    void load() override;
    void prelisten() override;

   private:
    Core::Api::Interface &m_core;
    Gtk::FileChooserWidget *m_files;
  };
}