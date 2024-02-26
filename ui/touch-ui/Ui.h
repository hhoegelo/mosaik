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

namespace Ui::Touch
{
  class Ui
  {
   public:
    Ui(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);

    void run();

   private:
    Core::Api::Interface &m_core;
    Dsp::Api::Display::Interface &m_dsp;
    Glib::RefPtr<Gtk::Application> m_app;
  };
}
