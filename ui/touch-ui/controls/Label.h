#pragma once

#include <gtkmm/label.h>
#include "core/Types.h"
#include "tools/SignalSlot.h"
#include <functional>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{

  class Label : public Gtk::Label
  {
   public:
    using Handler = std::function<void(Label& pThis, const Core::ParameterValue& in)>;
    Label(Core::Api::Interface& core, Core::TileId tileId, Core::ParameterId parameterId, const Handler& handler);

   private:
    Handler m_handler;
    Tools::Signals::Connection m_connection;
  };

}