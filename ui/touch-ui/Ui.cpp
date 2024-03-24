#include "Ui.h"
#include "Window.h"

namespace Ui::Touch
{
  Ui::Ui(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp)
      : m_core(core)
      , m_dsp(dsp)
      , m_app(Gtk::Application::create("com.mosaik.v4"))
      , m_window(std::make_unique<Window>(m_core, m_dsp))
  {
  }

  Ui::~Ui() = default;

  void Ui::run()
  {
    m_app->run(*m_window);
  }

  void Ui::attach(Gtk::Window &wnd)
  {
    m_app->signal_activate().connect([&] { m_app->add_window(wnd); });
  }

  Interface &Ui::getApi()
  {
    return *m_window;
  }
}
