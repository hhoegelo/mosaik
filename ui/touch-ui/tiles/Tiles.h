#include <ui/Types.h>
#include <core/Types.h>
#include <dsp/api/display/Interface.h>
#include <gtkmm/window.h>
#include <gtkmm/widget.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  Gtk::Widget* buildTiles(Gtk::Window& wnd, Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp,
                          std::function<Core::TileId(Row, Col)> map);
}