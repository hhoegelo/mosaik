
#include "GlobalTools.h"
#include "core/api/Interface.h"
#include "core/Types.h"
#include "KnobGrid.h"

#include <gtkmm/label.h>
#include <gtkmm/levelbar.h>

namespace Ui::Touch
{

  GlobalTools::GlobalTools(Core::Api::Interface &core)
  {
    auto knobs = Gtk::manage(new KnobGrid());

    auto addKnob = [&](const char *title, Core::ParameterId id, Knob k)
    { knobs->set(k, title, [&core, id] { return core.getParameterDisplay({}, id); }); };

    addKnob("Volume", Core::ParameterId::GlobalVolume, Knob::Center);
    addKnob("Tempo", Core::ParameterId::GlobalTempo, Knob::SouthEast);

    pack_start(*knobs);
  }

}