#include "FloatScaleButton.h"
#include "core/api/Interface.h"

namespace Ui::Touch
{
  FloatScaleButton::FloatScaleButton(Core::Api::Interface& core, Core::TileId tileId, Core::ParameterId parameterId,
                                     double min, double max, double stepSize, const std::string& img)
      : Gtk::ScaleButton(Gtk::IconSize(Gtk::ICON_SIZE_BUTTON), min, max, stepSize, { img })
  {
    auto connection = signal_value_changed().connect(
        [&core, tileId, parameterId](double v) { core.setParameter(tileId, parameterId, static_cast<float>(v)); });

    core.connect(tileId, parameterId,
                 [this, connection = std::move(connection)](const Core::ParameterValue& p) mutable
                 {
                   connection.block();
                   set_value(get<float>(p));
                   connection.unblock();
                 });
  }
}