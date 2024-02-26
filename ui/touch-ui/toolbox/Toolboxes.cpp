#include "Toolboxes.h"
#include "Pattern.h"
#include "ui/touch-ui/controls/FloatScaleButton.h"
#include <core/api/Interface.h>
#include <gtkmm/filechooserwidget.h>
#include <gtkmm/scale.h>
#include <iostream>

namespace Ui::Touch
{

  Toolboxes::Toolboxes(Core::Api::Interface &core)
      : m_core(core)
  {
    append_page(*buildGlobals(), "Globals");
    append_page(*buildFileBrowser(), "Browse");
    append_page(*Gtk::manage(new Pattern(core)), "Pattern");
  }

  Gtk::Widget *Toolboxes::buildScale(Core::TileId tile, Core::ParameterId id, double min, double max)
  {
    auto scale = Gtk::manage(new Gtk::Scale(Gtk::Orientation::ORIENTATION_HORIZONTAL));
    scale->set_range(min, max);
    auto connection = scale->signal_value_changed().connect(
        [this, scale, tile, id]() { m_core.setParameter(tile, id, static_cast<float>(scale->get_value())); });

    m_core.connect(tile, id,
                   [scale, connection = connection](const Core::ParameterValue &p) mutable
                   {
                     connection.block();
                     scale->set_value(get<float>(p));
                     connection.unblock();
                   });
    return scale;
  }

  Gtk::Widget *Toolboxes::buildGlobals()
  {
    auto globals = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));

    globals->add(*Gtk::manage(new Gtk::Label("Volume")));
    globals->add(*buildScale({}, Core::ParameterId::GlobalVolume, 0.0, 1.0));
    globals->add(*Gtk::manage(new Gtk::Label("Tempo")));
    globals->add(*buildScale({}, Core::ParameterId::GlobalTempo, 20, 240));

    return globals;
  }

  Gtk::Widget *Toolboxes::buildFileBrowser()
  {
    auto fileBrowser = Gtk::manage(new Gtk::FileChooserWidget(Gtk::FILE_CHOOSER_ACTION_OPEN));
    fileBrowser->signal_file_activated().connect(
        [fileBrowser, this]
        {
          for(auto tileId : m_core.getSelectedTiles())
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

    return fileBrowser;
  }
}
