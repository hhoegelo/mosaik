#include "Tiles.h"
#include "Tile.h"
#include <gtkmm/grid.h>

namespace Ui::Touch
{
  Gtk::Widget* buildTiles(Gtk::Window& wnd, Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp,
                          std::function<Core::TileId(Row, Col)> map)
  {
    auto grid = new Gtk::Grid();

    for(Row r = 0; r < NUM_TILE_ROWS; r++)
    {
      for(Col c = 0; c < NUM_TILE_COLUMNS; c++)
      {
        auto tile = Gtk::manage(new Tile(wnd, core, dsp, map(r, c)));
        grid->attach(*tile, r, c);
      }
    }

    return grid;
  }
}