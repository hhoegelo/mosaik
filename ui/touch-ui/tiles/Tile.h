#pragma once

#include <core/Types.h>
#include <gtkmm/grid.h>
#include <gtkmm/window.h>

namespace Core::Api
{
  class Interface;
}

namespace Dsp::Api::Display
{
  class Interface;
}

namespace Ui::Touch
{
  class Tile : public Gtk::Grid
  {
   public:
    Tile(Gtk::Window& wnd, Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Core::TileId tileId);
  };
}