#include "Ui.h"
#include "Window.h"
#include <gtkmm-4.0/gtkmm.h>

namespace Ui
{
  namespace Touch
  {
    Ui::Ui(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp)
        : m_core(core)
        , m_dsp(dsp)
    {
    }

    void Ui::run()
    {
      auto app = Gtk::Application::create("com.mosaik.v4");
      app->make_window_and_run<Window>(0, nullptr, m_core, m_dsp);
    }
  }
}
