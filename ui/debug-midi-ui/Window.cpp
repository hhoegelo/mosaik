#include "Window.h"
#include <gtkmm/grid.h>

namespace Ui::DebugMidi
{
  auto css = R"(
  .has-steps-indicator {
  }

  .has-steps-indicator.has-steps {
    background-color: #AA0000;
  }

  .is-playing-indicator {
  }

  .is-playing-indicator.is-playing {
    background: #AA0000;
  }

  .tile .volume {
    min-height: 3cm;
  }

  .waveform {
    min-height: 3cm;
    min-width: 3cm;
  }

  .tile {
	  border: 1px solid black;
    border-radius: 2px;
	  margin: 5px;
  }

  .tile.selected {
    border-color: red;
  }

  )";

  Window::Window(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp)
      : m_core(core)
      , m_dsp(dsp)
  {
  }

  void Window::build()
  {
    auto cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_data(css);
    Glib::RefPtr<Gtk::StyleContext> styleContext = Gtk::StyleContext::create();
    styleContext->add_provider_for_screen(Gdk::Screen::get_default(), cssProvider,
                                          GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    set_title("Mosaik Debug UI");
    set_border_width(10);
    auto screen = Gtk::manage(new Gtk::Grid());

    for(int i = 0; i < 16; i++)
    {
      screen->attach(*buildStep(i), i, 0);
      screen->attach(*buildStep(i + 16), 16, i);
      screen->attach(*buildStep(i + 32), 16 - i, 16);
      screen->attach(*buildStep(i + 48), 0, 16 - i);
    }

    add(*screen);
    show_all();
  }

  Gtk::Widget* Window::buildStep(int i)
  {
    return Gtk::manage(new Gtk::Button(std::to_string(1 + i)));
  }
}
