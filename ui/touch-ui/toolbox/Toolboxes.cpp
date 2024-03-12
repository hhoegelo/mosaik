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
    append_page(*(Gtk::manage(new Waveform(core))), "Wave");

    signal_switch_page().connect(
        [&sharedUiState](auto, auto idx)
        {
          g_printerr("Selecting page %d\n", idx);
          sharedUiState.select(static_cast<Ui::SharedState::Toolboxes>(idx));
          g_printerr("Selected page %d\n", idx);
        });

    m_selectedPageConnection = sharedUiState.connectSelectedToolbox(
        [this](auto t)
        {
          g_printerr("XXX Selecting page %d\n", t);
          auto n = static_cast<int>(t);
          auto c = this->get_current_page();
          if(c != n)
            this->set_current_page(n);
          g_printerr("XXX Selected page %d\n", t);
        });

    signal_switch_page().connect(
        [this, tileTools](Widget *w, auto)
        {
          if(w == tileTools)
            tileTools->onToolShown();
        });
  }
}
