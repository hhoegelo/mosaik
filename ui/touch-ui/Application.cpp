#include "Application.h"
#include "Window.h"

namespace Ui::Touch
{
  Application::Application(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp)
      : m_core(core)
      , m_dsp(dsp)
      , m_app(Gtk::Application::create("com.mosaik.v4"))
      , m_window(std::make_unique<Window>(m_core, m_dsp))
  {
    auto s = Gtk::Settings::get_default();
    s->property_gtk_theme_name() = "Adwaita";
    s->property_gtk_application_prefer_dark_theme() = true;
  }

  Application::~Application() = default;

  void Application::run()
  {
    m_app->run(*m_window);
  }

  void Application::attach(Gtk::Window &wnd)
  {
    m_app->signal_activate().connect([&] { m_app->add_window(wnd); });
  }

  Interface &Application::getApi()
  {
    return *m_window;
  }
}
