#pragma once

#include <gtkmm/box.h>
#include <gtkmm/window.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class Tile : public Gtk::Box
  {
   public:
    Tile(Gtk::Window& wnd, Core::Api::Interface& core, int tile);
  };
}