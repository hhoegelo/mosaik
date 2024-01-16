#pragma once

#include <core/Types.h>
#include <tools/SignalSlot.h>
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
    StepButton(Core::Api::Interface& core, int tileId, int step);

   private:
    Tools::Signals::Connection m_connection;
    Core::Pattern m_currentPattern {};
  };
}