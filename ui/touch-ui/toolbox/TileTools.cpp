#include "TileTools.h"
#include <core/api/Interface.h>

#include <gtkmm/filechooserwidget.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/levelbar.h>

namespace Ui::Touch
{
  TileTools::TileTools(Core::Api::Interface &core)
      : Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL)
      , m_core(core)
  {
    m_fileBrowser = Gtk::manage(new Gtk::FileChooserWidget(Gtk::FILE_CHOOSER_ACTION_OPEN));
    m_fileBrowser->signal_file_activated().connect(
        [this]
        {
          for(auto tileId : m_core.getSelectedTiles(nullptr))
            m_core.setParameter(tileId, Core::ParameterId::SampleFile, m_fileBrowser->get_filename());
        });

    m_fileBrowser->signal_current_folder_changed().connect(
        [this] { m_lastSelectedFolder = m_fileBrowser->get_current_folder_uri(); });

    pack_start(*m_fileBrowser);

    auto controls = Gtk::manage(new Gtk::Grid());

    auto gain = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    gain->add(*Gtk::manage(new Gtk::Label("Gain")));
    auto gainLevel = Gtk::manage(new Gtk::LevelBar());
    gainLevel->set_min_value(0);
    gainLevel->set_max_value(1.0);
    gain->add(*gainLevel);

    auto fadeInPos = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    fadeInPos->add(*Gtk::manage(new Gtk::Label("InPos")));
    fadeInPos->add(*Gtk::manage(new Gtk::LevelBar()));

    auto fadeInLen = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    fadeInLen->add(*Gtk::manage(new Gtk::Label("InLen")));
    fadeInLen->add(*Gtk::manage(new Gtk::LevelBar()));

    auto fadeOutPos = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    fadeOutPos->add(*Gtk::manage(new Gtk::Label("OutPos")));
    fadeOutPos->add(*Gtk::manage(new Gtk::LevelBar()));

    auto fadeOutLen = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    fadeOutLen->add(*Gtk::manage(new Gtk::Label("OutLen")));
    fadeOutLen->add(*Gtk::manage(new Gtk::LevelBar()));

    auto speed = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    speed->add(*Gtk::manage(new Gtk::Label("Speed")));
    auto speedLevel = Gtk::manage(new Gtk::LevelBar());
    speed->add(*speedLevel);

    auto balance = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    balance->add(*Gtk::manage(new Gtk::Label("Balance")));
    auto balanceLevel = Gtk::manage(new Gtk::LevelBar());
    balance->add(*balanceLevel);

    controls->attach(*balance, 0, 0, 1, 1);
    controls->attach(*speed, 4, 0, 1, 1);

    controls->attach(*fadeInPos, 1, 1, 1, 1);
    controls->attach(*fadeInLen, 3, 1, 1, 1);

    controls->attach(*gain, 2, 2, 1, 1);

    controls->attach(*fadeOutPos, 1, 3, 1, 1);
    controls->attach(*fadeOutLen, 3, 3, 1, 1);

    m_computations.add([this, gainLevel](auto p) { updateTileGain(p, gainLevel); });
    m_computations.add([this, speedLevel](auto p) { updateTileSpeed(p, speedLevel); });
    m_computations.add([this, balanceLevel](auto p) { updateTileBalance(p, balanceLevel); });

    pack_start(*controls);
  }

  void TileTools::onToolShown()
  {
    if(m_lastSelectedFolder)
      m_fileBrowser->set_current_folder_uri(m_lastSelectedFolder.value());
  }

  void TileTools::updateTileGain(Core::Api::Computation *c, Gtk::LevelBar *level)
  {
    level->set_value(std::get<float>(m_core.getFirstSelectedTileParameter(c, Core::ParameterId::Gain)));
  }

  void TileTools::updateTileSpeed(Core::Api::Computation *c, Gtk::LevelBar *level)
  {
    level->set_value(std::get<float>(m_core.getFirstSelectedTileParameter(c, Core::ParameterId::Speed)));
  }

  void TileTools::updateTileBalance(Core::Api::Computation *c, Gtk::LevelBar *level)
  {
    level->set_value(std::get<float>(m_core.getFirstSelectedTileParameter(c, Core::ParameterId::Balance)));
  }
}
