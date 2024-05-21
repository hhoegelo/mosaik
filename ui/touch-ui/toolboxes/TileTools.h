#pragma once

#include "ui/Types.h"
#include "GenericToolbox.h"
#include "tools/Expiration.h"
#include "ui/touch-ui/Interface.h"

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
    TileTools(ToolboxesInterface &toolboxes, Core::Api::Interface &core, Ui::Controller &controller);

    void up() override;
    void down() override;
    void inc() override;
    void dec() override;
    void load() override;

   private:
    void prelisten();

    Core::Api::Interface &m_core;
    Gtk::FileChooserWidget *m_files;
    Tools::DeferredComputations m_computations;
    std::string m_selection;
    Tools::Expiration m_prelistenDelay;
  };
}