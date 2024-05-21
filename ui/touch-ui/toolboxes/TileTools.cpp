#include "TileTools.h"
#include "ui/touch-ui/tools/WidgetTools.h"
#include <gtkmm/filechooserwidget.h>
#include <gtkmm/treepath.h>
#include <gtkmm/treeview.h>

using namespace std::chrono_literals;

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

  TileTools::TileTools(ToolboxesInterface &toolboxes, Core::Api::Interface &core, Ui::Controller &controller)
      : GenericMaximized(controller)
      , m_core(core)
      , m_prelistenDelay([this] { prelisten(); })
  {
    m_files = Gtk::manage(new Gtk::FileChooserWidget());
    auto home = getenv("HOME");
    auto music = Tools::format("/%s/Music/", home);
    m_selection = music;

    auto places = findChildWidget(m_files, "places_sidebar");
    places->set_visible(false);
    places->set_no_show_all();

    auto paths = findChildWidget(m_files, "browse_header_revealer");
    paths->set_visible(false);
    paths->set_no_show_all();

    pack_start(*m_files, true, true);

    m_computations.add(
        [this, &toolboxes, wasSelected = false]() mutable
        {
          auto isSelected = toolboxes.getSelectedToolbox() == Ui::Toolbox::Tile;
          if(wasSelected && !isSelected)
          {
            m_selection = m_files->get_current_folder();
          }
          else if(!wasSelected && isSelected)
          {
            m_files->set_current_folder(m_selection);
          }
          wasSelected = isSelected;
        });
  }

  void TileTools::up()
  {
    if(auto f = m_files->get_file())
      m_files->set_file(f->get_parent());
  }

  void TileTools::down()
  {
    if(auto f = m_files->get_file())
    {
      if(f->query_file_type() == Gio::FILE_TYPE_DIRECTORY)
      {
        m_files->set_current_folder_file(f);
      }
    }

    m_prelistenDelay.refresh(10ms);
  }

  void TileTools::inc()
  {
    navigate(m_files,
             [this](auto &p)
             {
               p.next();
               m_prelistenDelay.refresh(200ms);
             });
  }

  void TileTools::dec()
  {
    navigate(m_files,
             [this](auto &p)
             {
               p.prev();
               m_prelistenDelay.refresh(200ms);
             });
  }

  void TileTools::load()
  {
    m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::SampleFile, Core::Path(m_files->get_filename()));
  }

  void TileTools::prelisten()
  {
    m_core.setPrelistenSample(m_files->get_filename());
  }
}
