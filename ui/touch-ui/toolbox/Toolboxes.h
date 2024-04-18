#pragma once

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include "tools/ReactiveVar.h"
#include <ui/Types.h>
#include <ui/touch-ui/Interface.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class Waveform;
  class GlobalTools;
  class TileTools;
  class Steps;

  class Toolboxes : public Gtk::ScrolledWindow
  {
   public:
    explicit Toolboxes(Core::Api::Interface &core);

    Ui::Touch::Interface::Waveform &getWaveform() const;
    Ui::Touch::Interface::FileBrowser &getFileBrowser() const;

    ::Ui::Toolboxes getSelectedToolbox() const;

   private:
    Core::Api::Interface &m_core;

    Tools::ReactiveVar<::Ui::Toolboxes> m_selectedToolbox;
    Gtk::Box &m_box;
    TileTools &m_tileTools;
    Waveform &m_waveform;
  };
}
