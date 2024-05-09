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
  class Controller;

  namespace Touch
  {
    class Interface;
    class Window;

    class Application
    {
     public:
      Application(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp, Ui::Controller &controller);
      ~Application();

      void attach(Gtk::Window &wnd);
      void run();

      Interface &getApi();

     private:
      Glib::RefPtr<Gtk::Application> m_app;
      std::unique_ptr<Window> m_window;
    };
  }
}