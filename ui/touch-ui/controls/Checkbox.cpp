#include "Checkbox.h"
#include "core/api/Interface.h"

namespace Ui::Touch
{

  Checkbox::Checkbox(Core::Api::Interface& core, const std::string& name, Core::TileId tileId,
                     Core::ParameterId parameterId)
      : Gtk::CheckButton(name)
  {
    auto connection = signal_toggled().connect([&core, tileId, parameterId, this]
                                               { core.setParameter(tileId, parameterId, !m_state); });

    core.connect(tileId, parameterId,
                 [this, connection](const Core::ParameterValue& p) mutable
                 {
                   connection.block();
                   m_state = get<bool>(p);
                   this->set_active(m_state);
                   connection.unblock();
                 });
  }
}