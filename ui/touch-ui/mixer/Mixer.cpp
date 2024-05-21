#include <gtkmm/eventbox.h>
#include "Mixer.h"
#include "Channel.h"

namespace Ui::Touch
{
  Mixer::Mixer(Touch::Interface& touch, Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp,
               Ui::Controller& controller)
  {
    get_style_context()->add_class("mixer");

    set_column_homogeneous(true);
    set_row_homogeneous(true);

    for(uint8_t c = 0; c < NUM_CHANNELS; c++)
    {
      Core::Address address { c, {} };
      auto channel = Gtk::manage(new Channel(core, dsp, controller, address));
      auto e = Gtk::manage(new Gtk::EventBox());
      e->add_events(Gdk::EventMask::BUTTON_PRESS_MASK | Gdk::EventMask::POINTER_MOTION_MASK);

      e->signal_button_press_event().connect([&core, &touch, address](GdkEventButton*) { return false; });

      e->add(*channel);
      attach(*e, c, 0);
    }
  }

}