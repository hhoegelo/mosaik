#include "TileTools.h"
#include "core/api/Interface.h"
#include "core/Types.h"

#include <gtkmm/filechooserwidget.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/levelbar.h>
#include <gtkmm/eventbox.h>
#include <gdkmm/seat.h>
#include <gtkmm/treeview.h>

namespace Ui::Touch
{
  template <typename T> T *findChildWidget(Gtk::Widget *p)
  {
    if(auto found = dynamic_cast<T *>(p))
      return found;

    if(auto c = dynamic_cast<Gtk::Container *>(p))
      for(auto child : c->get_children())
        if(auto found = findChildWidget<T>(child))
          return found;

    return nullptr;
  }

  TileTools::TileTools(Core::Api::Interface &core)
      : Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL)
      , m_core(core)
  {
    int f = 50;

    m_fileBrowser = Gtk::manage(new Gtk::FileChooserWidget(Gtk::FILE_CHOOSER_ACTION_OPEN));
    m_fileBrowser->get_style_context()->add_class("file-browser");
    m_fileBrowser->signal_file_activated().connect(
        [this]
        {
          for(auto tileId : m_core.getSelectedTiles())
            m_core.setParameter(tileId, Core::ParameterId::SampleFile, m_fileBrowser->get_filename());
        });

    pack_start(*m_fileBrowser);

    auto controls = Gtk::manage(new Gtk::Grid());

    auto addParameter = [&](const char *title, Core::ParameterId id, int x, int y)
    {
      auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
      box->add(*Gtk::manage(new Gtk::Label(title)));
      auto level = Gtk::manage(new Gtk::Label());
      box->add(*level);
      controls->attach(*box, x, y, 1, 1);
      m_computations.add([this, level, id] { level->set_label(m_core.getFirstSelectedTileParameterDisplay(id)); });
    };

    addParameter("Gain", Core::ParameterId::Gain, 2, 2);
    addParameter("Speed", Core::ParameterId::Speed, 4, 0);
    addParameter("Balance", Core::ParameterId::Balance, 0, 0);
    addParameter("Shuffle", Core::ParameterId::Shuffle, 0, 3);

    pack_start(*controls);
  }

  void TileTools::up()
  {
    if(auto f = m_fileBrowser->get_file())
      m_fileBrowser->set_file(f->get_parent());
  }

  void TileTools::down()
  {
    if(auto f = m_fileBrowser->get_file())
      m_fileBrowser->set_current_folder_file(f);
  }

  void TileTools::inc()
  {
    navigate([](auto &p) { p.next(); });
  }

  void TileTools::dec()
  {
    navigate([](auto &p) { p.prev(); });
  }

  void TileTools::load()
  {
    m_core.setParameter(m_core.getSelectedTiles().front(), Core::ParameterId::SampleFile,
                        Core::Path(m_fileBrowser->get_filename()));
  }

  void TileTools::prelisten()
  {
    m_core.setPrelistenSample(m_fileBrowser->get_filename());
  }

  void TileTools::navigate(const std::function<void(Gtk::TreePath &)> &cb)
  {
    auto tree = findChildWidget<Gtk::TreeView>(m_fileBrowser);
    auto selection = tree->get_selection();
    auto selectedRows = selection->get_selected_rows();
    if(!selectedRows.empty())
    {
      auto p = selectedRows[0];
      cb(p);
      selection->select(p);
      tree->scroll_to_row(p);
    }
  }
}
