#include "MainPlayground.h"
#include "KnobGrid.h"
#include "SoftButtonGrid.h"
#include "core/api/Interface.h"
#include <ui/touch-ui/Display.h>

namespace Ui::Touch
{
  MainPlayground::MainPlayground(Core::Api::Interface &core)
  {
    auto knobs = Gtk::manage(new KnobGrid());

    auto addKnob = [&](const char *title, Core::ParameterId id, Knob k)
    { knobs->set(k, title, [&core, id] { return getDisplayValue(core, {}, id); }); };

    addKnob("1", Core::ParameterId::MainPlayground1, Knob::Leftmost);
    addKnob("2", Core::ParameterId::MainPlayground2, Knob::Rightmost);
    addKnob("3", Core::ParameterId::MainPlayground3, Knob::NorthWest);
    addKnob("4", Core::ParameterId::MainPlayground4, Knob::NorthEast);
    addKnob("5", Core::ParameterId::MainPlayground5, Knob::Center);
    addKnob("6", Core::ParameterId::MainPlayground6, Knob::SouthWest);
    addKnob("7", Core::ParameterId::MainPlayground7, Knob::SouthEast);

    pack_start(*knobs);
  }
}
