#include <gtkmm/eventbox.h>
#include "Tiles.h"
#include "Tile.h"

namespace Ui::Touch
{
  Tiles::Tiles(Touch::Interface& touch, Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp,
               Ui::Controller& controller)
      : SectionWrapper(touch)
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

    for(Row r = 0; r < NUM_TILE_ROWS; r++)
    {
      for(Col c = 0; c < NUM_TILE_COLUMNS; c++)
      {
        auto tileId = r * NUM_TILE_COLUMNS + c;
        auto tile = Gtk::manage(new Tile(core, dsp, controller, tileId));
        auto e = Gtk::manage(new Gtk::EventBox());
        e->add_events(Gdk::EventMask::BUTTON_PRESS_MASK | Gdk::EventMask::POINTER_MOTION_MASK);

        e->signal_button_press_event().connect(
            [&core, &touch, tileId](GdkEventButton*)
            {
              if(touch.getToolboxes().getSelectedToolbox() == Toolbox::Mute)
              {
                auto muted = std::get<bool>(core.getParameter(tileId, Core::ParameterId::Mute));
                core.setParameter(tileId, Core::ParameterId::Mute, !muted);
              }
              else
              {
                core.setParameter(tileId, Core::ParameterId::Selected, true);
              }
              return false;
            });

        e->add(*tile);
        attach(*e, r, c);
      }
    }
  }

}