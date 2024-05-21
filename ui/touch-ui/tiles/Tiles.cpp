#include <gtkmm/eventbox.h>
#include "ui/touch-ui/Interface.h"
#include "Tiles.h"
#include "Tile.h"

namespace Ui::Touch
{
  Tiles::Tiles(Touch::Interface& touch, Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp,
               Ui::Controller& controller)
  {
    get_style_context()->add_class("tiles");

    set_column_homogeneous(true);
    set_row_homogeneous(true);

    m_computations.add(
        [this, &touch]
        {
          if(touch.getToolboxes().getSelectedToolbox() == Ui::Toolbox::Mute)
            get_style_context()->add_class("mute-toolbox");
          else
            get_style_context()->remove_class("mute-toolbox");
        });

    for(uint8_t t = 0; t < NUM_TILES; t++)
    {
      Core::Address address { t };
      auto tile = Gtk::manage(new Tile(core, dsp, controller, address));
      auto e = Gtk::manage(new Gtk::EventBox());
      e->add_events(Gdk::EventMask::BUTTON_PRESS_MASK | Gdk::EventMask::POINTER_MOTION_MASK);

      e->signal_button_press_event().connect(
          [&core, &touch, address](GdkEventButton*)
          {
            if(touch.getToolboxes().getSelectedToolbox() == Toolbox::Mute)
            {
              auto muted = std::get<bool>(core.getParameter(address, Core::ParameterId::Mute));
              core.setParameter(address, Core::ParameterId::Mute, !muted);
            }
            else
            {
              core.setParameter(address, Core::ParameterId::Selected, true);
              auto sample = std::get<Core::Path>(core.getParameter(address, Core::ParameterId::SampleFile));
              if(exists(sample))
                core.setPrelistenSample(sample);
            }
            return false;
          });

      e->add(*tile);
      attach(*e, t / NUM_TILES_Y, t % NUM_TILES_Y);
    }
  }

}