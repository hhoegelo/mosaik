#pragma once

#include "core/Types.h"
#include <gtkmm/button.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class StepButton : public Gtk::Button
  {
   public:
    StepButton(Core::Api::Interface& core, Core::TileId tileId, int step);

   private:
    Core::Pattern m_currentPattern {};
  };
}