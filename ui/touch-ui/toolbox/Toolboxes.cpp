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

    m_selectedPageConnection = sharedUiState.connectSelectedToolbox(
        [this](auto t)
        {
          auto n = static_cast<int>(t);
          auto c = this->get_current_page();
          if(c != n)
            this->set_current_page(n);
        });

    signal_switch_page().connect(
        [this, tileTools](Widget *w, auto)
        {
          if(w == tileTools)
            tileTools->onToolShown();
        });
  }
}
