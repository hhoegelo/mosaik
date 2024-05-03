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

    for(Row r = 0; r < NUM_TILE_ROWS; r++)
    {
      for(Col c = 0; c < NUM_TILE_COLUMNS; c++)
      {
        auto tile = Gtk::manage(new Tile(core, dsp, controller, r * NUM_TILE_COLUMNS + c));
        attach(*tile, r, c);
      }
    }
  }

}