
#include "GlobalTools.h"
#include "core/api/Interface.h"
#include "core/Types.h"

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/levelbar.h>

namespace Ui::Touch
{

  GlobalTools::GlobalTools(Core::Api::Interface &core)
      : m_core(core)
  {
    auto volBox = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    volBox->add(*Gtk::manage(new Gtk::Label("Volume")));
    volBox->add(*buildLevel(Core::ParameterId::GlobalVolume));

    auto tempoBox = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    tempoBox->add(*Gtk::manage(new Gtk::Label("Tempo")));
    tempoBox->add(*buildLevel(Core::ParameterId::GlobalTempo));

    auto buildEmptyBox = [&]
    {
      auto emptyBox = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
      emptyBox->add(*Gtk::manage(new Gtk::Label("unused")));

      return emptyBox;
    };

    attach(*buildEmptyBox(), 0, 0, 1, 1);
    attach(*buildEmptyBox(), 4, 0, 1, 1);

    attach(*buildEmptyBox(), 1, 1, 1, 1);
    attach(*buildEmptyBox(), 3, 1, 1, 1);

    attach(*volBox, 2, 2, 1, 1);

    attach(*buildEmptyBox(), 1, 3, 1, 1);
    attach(*tempoBox, 3, 3, 1, 1);
  }

  Gtk::Label *GlobalTools::buildLevel(Core::ParameterId id)
  {
    auto level = Gtk::manage(new Gtk::Label());
    m_computations.add([this, id, level] { level->set_label(m_core.getParameterDisplay({}, id)); });
    return level;
  }

}