#include "Window.h"
#include "ui/touch-ui/tiles/Tiles.h"
#include "ui/touch-ui/controls/FloatScaleButton.h"
#include "ui/touch-ui/toolbox/Toolboxes.h"
#include <gtkmm/grid.h>

namespace Ui::Touch
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
  {
    auto cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_data(css);
    Glib::RefPtr<Gtk::StyleContext> styleContext = Gtk::StyleContext::create();
    styleContext->add_provider_for_screen(Gdk::Screen::get_default(), cssProvider,
                                          GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    set_title("Mosaik");
    set_border_width(10);

    auto volume
        = Gtk::manage(new FloatScaleButton(core, {}, Core::ParameterId::GlobalVolume, 0.0, 1.0, 0.01, "stock_volume"));

    auto tempo = Gtk::manage(
        new FloatScaleButton(core, {}, Core::ParameterId::GlobalTempo, 20, 240, 0.5, "speedometer-symbolic"));

    auto screen = Gtk::manage(new Gtk::Grid());
    screen->attach(*volume, 0, 0, 1, 1);
    screen->attach(*tempo, 1, 0, 1, 1);

    auto tiles = new Tiles(core, dsp, [](Row r, Col c) { return r * NUM_TILE_COLUMNS + c; });
    screen->attach(*tiles, 0, 1, 10, 10);

    auto toolboxes = new Toolboxes(core);
    screen->attach(*toolboxes, 10, 0, 5, 11);

    add(*screen);
    show_all();
  }
}
