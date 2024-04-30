#pragma once

#include "tools/ReactiveVar.h"
#include "core/Types.h"
#include <gtkmm/box.h>

namespace Gtk
{
  class LevelBar;
}

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{

  class GlobalTools : public Gtk::Box
  {
   public:
    explicit GlobalTools(Core::Api::Interface &core);
  };

}