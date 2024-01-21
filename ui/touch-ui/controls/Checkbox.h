#pragma once

#include <gtkmm/checkbutton.h>
#include "core/Types.h"

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{

  class Checkbox : public Gtk::CheckButton
  {
   public:
    Checkbox(Core::Api::Interface& core, const std::string& name, Core::TileId tileId, Core::ParameterId parameterId);

   private:
    bool m_state = false;
  };

}