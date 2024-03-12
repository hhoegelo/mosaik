#pragma once

#include <core/Types.h>
#include <gtkmm/grid.h>
#include "tools/SignalSlot.h"

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
    Gtk::LevelBar *buildLevel(Core::ParameterId id, double min, double max);

    Core::Api::Interface &m_core;
    std::vector<Tools::Signals::Connection> m_connections;
  };

}