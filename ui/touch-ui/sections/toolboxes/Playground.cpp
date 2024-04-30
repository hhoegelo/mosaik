#include "Playground.h"
#include "KnobGrid.h"
#include "SoftButtonGrid.h"
#include "core/api/Interface.h"
#include <ui/touch-ui/Display.h>

namespace Ui::Touch
{
  Playground::Playground(Core::Api::Interface &core)
  {
    auto knobs = Gtk::manage(new KnobGrid());

    auto addKnob = [&](const char *title, Core::ParameterId id, Knob k)
    { knobs->set(k, title, [&core, id] { return getDisplayValue(core, id); }); };

    addKnob("1", Core::ParameterId::Playground1, Knob::Leftmost);
    addKnob("2", Core::ParameterId::Playground2, Knob::Rightmost);
    addKnob("3", Core::ParameterId::Playground3, Knob::NorthWest);
    addKnob("4", Core::ParameterId::Playground4, Knob::NorthEast);
    addKnob("5", Core::ParameterId::Playground5, Knob::Center);
    addKnob("6", Core::ParameterId::Playground6, Knob::SouthWest);
    addKnob("7", Core::ParameterId::Playground7, Knob::SouthEast);

    pack_start(*knobs);
  }
}
