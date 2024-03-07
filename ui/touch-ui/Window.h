#pragma once

#include <gtkmm-3.0/gtkmm.h>

namespace Core::Api
{
  class Interface;
}

namespace Dsp::Api::Display
{
  class Interface;
}

namespace Ui
{
  class SharedState;

  namespace Touch
  {
    class Window : public Gtk::Window
    {
     public:
      Window(SharedState &sharedUiState, Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);
      void build();

     private:
      SharedState &m_sharedUiState;
      Core::Api::Interface &m_core;
      Dsp::Api::Display::Interface &m_dsp;
    };
  }
}