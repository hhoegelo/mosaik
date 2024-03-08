#include "Toolboxes.h"
#include <ui/touch-ui/controls/FloatScaleButton.h>
#include <ui/SharedState.h>
#include <core/api/Interface.h>
#include <gtkmm/filechooserwidget.h>
#include <iostream>
#include <gtkmm/levelbar.h>

namespace Ui::Touch
{

  Toolboxes::Toolboxes(SharedState &sharedUiState, Core::Api::Interface &core)
      : m_core(core)
  {
    append_page(*buildGlobals(), "Globals");
    append_page(*buildTileToolbox(), "Tile");

    signal_switch_page().connect(
        [&sharedUiState](auto, auto idx)
        {
          // foo
          sharedUiState.select(static_cast<Ui::SharedState::Toolboxes>(idx));
        });

    m_connections.push_back(sharedUiState.connectSelectedToolbox(
        [this](auto t)
        {
          auto n = static_cast<int>(t);
          if(this->get_current_page() != n)
            this->set_current_page(n);
        }));
  }

  Gtk::Widget *Toolboxes::buildLevel(Core::TileId tile, Core::ParameterId id, double min, double max)
  {
    auto level = Gtk::manage(new Gtk::LevelBar());
    level->set_min_value(min);
    level->set_max_value(max);

    m_connections.push_back(
        m_core.connect(tile, id, [level](const Core::ParameterValue &p) { level->set_value(get<float>(p)); }));

    return level;
  }

  Gtk::Widget *Toolboxes::buildGlobals()
  {
    auto globals = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));

    globals->add(*Gtk::manage(new Gtk::Label("Volume")));
    globals->add(*buildLevel({}, Core::ParameterId::GlobalVolume, 0.0, 1.0));
    globals->add(*Gtk::manage(new Gtk::Label("Tempo")));
    globals->add(*buildLevel({}, Core::ParameterId::GlobalTempo, 20, 240));
    globals->add(*Gtk::manage(new Gtk::Label("Shuffle")));
    globals->add(*buildLevel({}, Core::ParameterId::GlobalShuffle, 0.0, 1.0));

    return globals;
  }

  Gtk::Widget *Toolboxes::buildTileToolbox()
  {
    auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    auto fileBrowser = Gtk::manage(new Gtk::FileChooserWidget(Gtk::FILE_CHOOSER_ACTION_OPEN));
    fileBrowser->signal_file_activated().connect(
        [fileBrowser, this]
        {
          for(auto tileId : m_core.getSelectedTiles(nullptr))
            m_core.setParameter(tileId, Core::ParameterId::SampleFile, fileBrowser->get_filename());
        });

    fileBrowser->signal_current_folder_changed().connect(
        [this, fileBrowser] { m_lastSelectedFolder = fileBrowser->get_current_folder_file(); });

    this->signal_switch_page().connect(
        [this, fileBrowser](Widget *w, auto)
        {
          if(w == fileBrowser && m_lastSelectedFolder && m_lastSelectedFolder->query_exists())
            fileBrowser->set_current_folder_file(m_lastSelectedFolder);
        });

    box->pack_start(*fileBrowser);

    auto controlBox = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));

    // Tile Gain
    controlBox->pack_start(*Gtk::manage(new Gtk::Label("Gain")), false, false);

    auto level = Gtk::manage(new Gtk::LevelBar());
    level->set_min_value(0);
    level->set_max_value(1);
    controlBox->pack_start(*level, false, false);

    showTileGain(level);

    box->pack_start(*controlBox);

    return box;
  }

  void Toolboxes::showTileGain(Gtk::LevelBar *level)
  {
    m_tilePageGain = std::make_unique<Core::Api::Computation>();
    auto v = m_core.getFirstSelectedTileParameter(m_tilePageGain.get(), Core::ParameterId::Gain);
    level->set_value(std::get<float>(v));
    m_tilePageGain->refresh([this, level] { showTileGain(level); });
  }
}
