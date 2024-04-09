#pragma once

#include <ui/Types.h>
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
  namespace Touch
  {
    class Interface;
    class Window;

    class Application
    {
     public:
      Application(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);
      ~Application();

      void attach(Gtk::Window &wnd);
      void run();

      Interface &getApi();

     private:
      Core::Api::Interface &m_core;
      Dsp::Api::Display::Interface &m_dsp;
      Glib::RefPtr<Gtk::Application> m_app;

      std::unique_ptr<Window> m_window;
    };
  }
}