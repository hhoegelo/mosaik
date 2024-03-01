#include "Label.h"
#include "core/api/Interface.h"

namespace Ui::Touch
{

  Label::Label(Core::Api::Interface& core, Core::TileId tileId, Core::ParameterId parameterId, const Handler& handler)
      : Gtk::Label()
      , m_handler(handler)
  {
    m_connection = core.connect(tileId, parameterId, [this, &handler](const auto& v) { handler(*this, v); });
  }
}