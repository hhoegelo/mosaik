#include "Ui.h"
#include "Window.h"
#include <ui/debug-midi-ui/Window.h>

namespace Ui::Touch
{
  Ui::Ui(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp)
      : m_core(core)
      , m_dsp(dsp)
      , m_app(Gtk::Application::create("com.mosaik.v4"))
  {
  }

  void Ui::run()
  {
    Window win(m_core, m_dsp);
    win.build();

    DebugMidi::Window debugMidi(m_core, m_dsp);
    debugMidi.build();

    m_app->signal_activate().connect([&] { m_app->add_window(debugMidi); });
    m_app->run(win);
  }
}
