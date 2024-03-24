#pragma once

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include "tools/ReactiveVar.h"
#include <ui/Types.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class Waveform;
  class GlobalTools;
  class TileTools;

  class Toolboxes : public Gtk::ScrolledWindow
  {
   public:
    explicit Toolboxes(Core::Api::Interface &core);

    void incZoom(int inc);
    void incScroll(int inc);
    ::Ui::Toolboxes getSelectedToolbox() const;
    double getWaveformFramesPerPixel() const;

   private:
    Core::Api::Interface &m_core;

    Tools::ReactiveVar<::Ui::Toolboxes> m_selectedToolbox;
    Gtk::Box &m_box;
    GlobalTools &m_globalTools;
    TileTools &m_tileTools;
    Waveform &m_waveform;
  };
}
