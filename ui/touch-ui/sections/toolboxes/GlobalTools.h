#pragma once

#include "tools/ReactiveVar.h"
#include "core/Types.h"
#include <gtkmm/grid.h>

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

  class GlobalTools : public Gtk::Grid
  {
   public:
    explicit GlobalTools(Core::Api::Interface &core);

   private:
    Gtk::Label *buildLevel(Core::ParameterId id);

    Core::Api::Interface &m_core;
    Tools::DeferredComputations m_computations;
  };

}