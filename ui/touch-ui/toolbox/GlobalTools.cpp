
#include "GlobalTools.h"
#include <core/api/Interface.h>

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/levelbar.h>

namespace Ui::Touch
{

  GlobalTools::GlobalTools(Core::Api::Interface &core)
      : m_core(core)
      , m_computations(Glib::MainContext::get_default())
  {
    auto volBox = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    volBox->add(*Gtk::manage(new Gtk::Label("Volume")));
    volBox->add(*buildLevel(Core::ParameterId::GlobalVolume, 0.0, 1.0));

    auto tempoBox = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    tempoBox->add(*Gtk::manage(new Gtk::Label("Tempo")));
    tempoBox->add(*buildLevel(Core::ParameterId::GlobalTempo, 20, 240));

    auto shuffleBox = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    shuffleBox->add(*Gtk::manage(new Gtk::Label("Shuffle")));
    shuffleBox->add(*buildLevel(Core::ParameterId::GlobalShuffle, 0.0, 1.0));

    auto buildEmptyBox = [&]
    {
      auto emptyBox = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
      emptyBox->add(*Gtk::manage(new Gtk::Label("unused")));
      emptyBox->add(*buildLevel(Core::ParameterId::Unused, 0.0, 1.0));
      return emptyBox;
    };

    attach(*buildEmptyBox(), 0, 0, 1, 1);
    attach(*buildEmptyBox(), 4, 0, 1, 1);

    attach(*buildEmptyBox(), 1, 1, 1, 1);
    attach(*shuffleBox, 3, 1, 1, 1);

    attach(*volBox, 2, 2, 1, 1);

    attach(*buildEmptyBox(), 1, 3, 1, 1);
    attach(*tempoBox, 3, 3, 1, 1);
  }

  Gtk::LevelBar *GlobalTools::buildLevel(Core::ParameterId id, double min, double max)
  {
    auto level = Gtk::manage(new Gtk::LevelBar());
    level->set_min_value(min);
    level->set_max_value(max);

    if(id != Core::ParameterId::Unused)
      m_computations.add([this, id, level] { level->set_value(get<float>(m_core.getParameter({}, id))); });

    return level;
  }

}