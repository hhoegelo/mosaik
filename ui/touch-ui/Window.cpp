#include "Window.h"
#include "ui/touch-ui/tiles/Tiles.h"
#include "ui/touch-ui/controls/FloatScaleButton.h"

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
  )";

  Window::Window(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp)
  {
    auto cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_data(css);
    Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
    Glib::RefPtr<Gtk::StyleContext> styleContext = Gtk::StyleContext::create();
    styleContext->add_provider_for_screen(screen, cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    set_title("Mosaik");
    set_border_width(10);

    Gtk::Box globals(Gtk::ORIENTATION_HORIZONTAL, 2);
    FloatScaleButton volume(core, {}, Core::ParameterId::GlobalVolume, 0.0, 1.0, 0.01, "stock_volume");
    globals.pack_start(volume, Gtk::PACK_SHRINK);

    FloatScaleButton tempo(core, {}, Core::ParameterId::GlobalTempo, 20, 240, 0.5, "speedometer-symbolic");
    globals.pack_start(tempo, Gtk::PACK_SHRINK);

    auto stack = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 2));
    stack->pack_start(globals, Gtk::PACK_SHRINK);

    stack->pack_start(*buildTiles(*this, core, dsp, [](Row r, Col c) { return r * NUM_TILE_COLUMNS + c; }),
                      Gtk::PACK_SHRINK);

    add(*stack);
    show_all();
  }
}
