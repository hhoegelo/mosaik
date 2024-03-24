#pragma once

#include <ui/touch-ui/Interface.h>
#include <gtkmm-3.0/gtkmm.h>

namespace Core::Api
{
  class Interface;
}

namespace Dsp::Api::Display
{
  class Interface;
}

namespace Ui::Touch
{
  class Tiles;
  class Toolboxes;

  class Window : public Gtk::Window, public Interface
  {
   public:
    Window(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);
    ~Window() override;

    void incWaveformZoom(int inc) override;
    void incWaveformScroll(int inc) override;
    ::Ui::Toolboxes getSelectedToolbox() const override;
    double getWaveformFramesPerPixel() const override;

   private:
    void build();

    Core::Api::Interface &m_core;
    Dsp::Api::Display::Interface &m_dsp;

    Tiles *m_tiles = nullptr;
    Toolboxes *m_toolboxes = nullptr;
  };
}
