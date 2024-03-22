#include "Toolboxes.h"
#include "GlobalTools.h"
#include "TileTools.h"
#include "Waveform.h"
#include <ui/SharedState.h>
#include <gtkmm/label.h>
#include <gtkmm/eventbox.h>

namespace Ui::Touch
{
  static void addToolbox(SharedState &sharedUiState, SharedState::Toolboxes s, Gtk::Box *box, const std::string &title,
                         Gtk::Widget *child)
  {
    child = Gtk::manage(child);

    auto events = Gtk::manage(new Gtk::EventBox());
    auto section = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    auto label = Gtk::manage(new Gtk::Label(title));
    label->get_style_context()->add_class("header");
    section->pack_start(*label, false, true);
    section->pack_start(*child, true, true);
    section->get_style_context()->add_class("section");
    events->add(*section);

    box->pack_start(*events, true, true);

    events->add_events(Gdk::BUTTON_PRESS_MASK);

    events->signal_button_press_event().connect(
        [&sharedUiState, s](GdkEventButton *event)
        {
          sharedUiState.select(s);
          return false;
        });
  }

  Toolboxes::Toolboxes(SharedState &sharedUiState, Core::Api::Interface &core)
      : m_core(core)
  {
    get_style_context()->add_class("toolboxes");

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

    addToolbox(sharedUiState, SharedState::Toolboxes::Global, box, "Global", new GlobalTools(core));
    addToolbox(sharedUiState, SharedState::Toolboxes::Tile, box, "Tile", new TileTools(core));
    addToolbox(sharedUiState, SharedState::Toolboxes::Waveform, box, "Wave", new Waveform(sharedUiState, core));

    this->add(*box);
  }
}
