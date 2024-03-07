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
    class Ui
    {
     public:
      Ui(SharedState &sharedUiState, Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);

      void attach(Gtk::Window &wnd);
      void run();

     private:
      SharedState &m_sharedUiState;
      Core::Api::Interface &m_core;
      Dsp::Api::Display::Interface &m_dsp;
      Glib::RefPtr<Gtk::Application> m_app;
    };
  }
}