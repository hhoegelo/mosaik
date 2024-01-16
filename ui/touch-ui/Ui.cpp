#include "Ui.h"
#include "Window.h"
#include <gtkmm-3.0/gtkmm.h>

namespace Ui::Touch
{
  Ui::Ui(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp)
      : m_core(core)
      , m_dsp(dsp)
  {
  }

  void Ui::run()
  {
    auto app = Gtk::Application::create("com.mosaik.v4");
    Window win(m_core, m_dsp);
    app->run(win);
  }
}
