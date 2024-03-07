#include "Ui.h"
#include "Window.h"

namespace Ui::Touch
{
  Ui::Ui(SharedState &sharedUiState, Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp)
      : m_sharedUiState(sharedUiState)
      , m_core(core)
      , m_dsp(dsp)
      , m_app(Gtk::Application::create("com.mosaik.v4"))
  {
  }

  void Ui::run()
  {
    Window win(m_sharedUiState, m_core, m_dsp);
    win.build();
    m_app->run(win);
  }

  void Ui::attach(Gtk::Window &wnd)
  {
    m_app->signal_activate().connect([&] { m_app->add_window(wnd); });
  }
}
