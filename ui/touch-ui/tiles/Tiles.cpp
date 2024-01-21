#include "Tiles.h"
#include "Tile.h"

namespace Ui::Touch
{
  Tiles::Tiles(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, std::function<Core::TileId(Row, Col)> map)
  {
    for(Row r = 0; r < NUM_TILE_ROWS; r++)
    {
      for(Col c = 0; c < NUM_TILE_COLUMNS; c++)
      {
        auto tile = Gtk::manage(new Tile(core, dsp, map(r, c)));
        attach(*tile, r, c);
      }
    }
  }
}