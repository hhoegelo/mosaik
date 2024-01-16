#include "Window.h"
#include "Tile.h"
#include "FloatScaleButton.h"

namespace Ui::Touch
{
  Window::Window(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp)
  {
    set_title("Mosaik");
    set_border_width(10);

    auto globals = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 2));
    globals->pack_start(
        *Gtk::manage(new FloatScaleButton(core, {}, Core::ParameterId::GlobalVolume, 0.0, 1.0, 0.01, "stock_volume")),
        Gtk::PACK_SHRINK);

    globals->pack_start(*Gtk::manage(new FloatScaleButton(core, {}, Core::ParameterId::GlobalTempo, 20, 240, 0.5,
                                                          "speedometer-symbolic")),
                        Gtk::PACK_SHRINK);

    auto stack = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 2));
    stack->pack_start(*globals, Gtk::PACK_SHRINK);

    for(auto c = 0; c < NUM_TILES; c++)
      stack->pack_start(*Gtk::manage(new Tile(*this, core, c)), Gtk::PACK_SHRINK);

    add(*stack);
    show_all();
  }
}
