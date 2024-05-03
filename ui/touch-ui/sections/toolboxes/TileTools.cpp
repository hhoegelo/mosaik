#include "TileTools.h"
#include <ui/touch-ui/tools/WidgetTools.h>
#include <gtkmm/filechooserwidget.h>
#include <gtkmm/treepath.h>
#include <gtkmm/treeview.h>

namespace Ui::Touch
{
  namespace
  {
    void navigate(Gtk::FileChooserWidget *files, const std::function<void(Gtk::TreePath &)> &cb)
    {
      auto tree = findChildWidget<Gtk::TreeView>(files);
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

  TileTools::TileTools(Core::Api::Interface &core, Ui::Controller &controller)
      : GenericMaximized(controller)
      , m_core(core)
  {
    m_files = Gtk::manage(new Gtk::FileChooserWidget());
    auto home = getenv("HOME");
    auto music = Tools::format("%s/Music/", home);
    m_files->set_current_folder(music);

    pack_start(*m_files, true, true);

    auto places = findChildWidget(this, "GtkPlacesSidebar");
    places->set_visible(false);
    places->set_no_show_all();
  }

  void TileTools::up()
  {
    if(auto f = m_files->get_file())
      m_files->set_file(f->get_parent());
  }

  void TileTools::down()
  {
    if(auto f = m_files->get_file())
      m_files->set_current_folder_file(f);
  }

  void TileTools::inc()
  {
    navigate(m_files, [](auto &p) { p.next(); });
  }

  void TileTools::dec()
  {
    navigate(m_files, [](auto &p) { p.prev(); });
  }

  void TileTools::load()
  {
    m_core.setParameter(m_core.getSelectedTiles().front(), Core::ParameterId::SampleFile,
                        Core::Path(m_files->get_filename()));
  }

  void TileTools::prelisten()
  {
    m_core.setPrelistenSample(m_files->get_filename());
  }

}
