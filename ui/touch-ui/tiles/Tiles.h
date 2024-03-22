#include <ui/Types.h>
#include <core/Types.h>
#include <gtkmm/grid.h>

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
  class Tiles : public Gtk::Grid
  {
   public:
    Tiles(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp);
  };

}