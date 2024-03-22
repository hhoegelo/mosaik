#include "Toolboxes.h"
#include "GlobalTools.h"
#include "TileTools.h"
#include "Waveform.h"
#include <ui/SharedState.h>

namespace Ui::Touch
{
  Toolboxes::Toolboxes(SharedState &sharedUiState, Core::Api::Interface &core)
      : m_core(core)
  {
    auto tileTools = new TileTools(core);
    append_page(*(Gtk::manage(new GlobalTools(core))), "Globals");
    append_page(*(Gtk::manage(tileTools)), "Tile");
    append_page(*(Gtk::manage(new Waveform(sharedUiState, core))), "Wave");

    signal_switch_page().connect([&sharedUiState](auto, auto idx)
                                 { sharedUiState.select(static_cast<Ui::SharedState::Toolboxes>(idx)); });
  }
}
