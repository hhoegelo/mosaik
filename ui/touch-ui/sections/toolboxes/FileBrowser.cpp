#include "FileBrowser.h"
#include "tools/Format.h"
#include <core/api/Interface.h>

#include <giomm.h>

namespace Ui::Touch
{
  struct Columns : public Gtk::TreeModel::ColumnRecord
  {
    Columns()
    {
      add(col_name);
      add(col_data);
    }

    Gtk::TreeModelColumn<std::string> col_name;
    Gtk::TreeModelColumn<Glib::RefPtr<Gio::File>> col_data;
  } c_columns;

  FileBrowser::FileBrowser(Core::Api::Interface &core)
      : m_core(core)
      , m_store(Gtk::ListStore::create(c_columns))
  {
    get_style_context()->add_class("file-browser");

    set_model(m_store);
    append_column("Name", c_columns.col_name);

    auto home = getenv("HOME");
    auto music = Tools::format("%s/Music/", home);
    setFolder(Gio::File::create_for_path(music));
  }

  void FileBrowser::up()
  {
    setFolder(m_currentFolder->get_parent());
  }

  void FileBrowser::down()
  {
    if(auto it = get_selection()->get_selected())
    {
      Glib::RefPtr<Gio::File> d = it->get_value(c_columns.col_data);

      if(d->query_file_type() == Gio::FILE_TYPE_DIRECTORY)
        setFolder(d);
    }
  }

  void FileBrowser::inc()
  {
    navigate([](auto &p) { p.next(); });
  }

  void FileBrowser::dec()
  {
    navigate([](auto &p) { p.prev(); });
  }

  void FileBrowser::load()
  {
    if(auto it = get_selection()->get_selected())
    {
      Glib::RefPtr<Gio::File> d = it->get_value(c_columns.col_data);
      if(d && d->query_file_type() == Gio::FileType::FILE_TYPE_REGULAR)
        m_core.setParameter(m_core.getSelectedTiles().front(), Core::ParameterId::SampleFile,
                            Core::Path(d->get_path()));
    }
  }

  void FileBrowser::prelisten()
  {
    if(auto it = get_selection()->get_selected())
    {
      Glib::RefPtr<Gio::File> d = it->get_value(c_columns.col_data);
      if(d && d->query_file_type() == Gio::FileType::FILE_TYPE_REGULAR)
        m_core.setPrelistenSample(d->get_path());
    }
  }

  void FileBrowser::navigate(const std::function<void(Gtk::TreePath &)> &cb)
  {
    auto selection = get_selection();
    auto selectedRows = selection->get_selected_rows();
    if(!selectedRows.empty())
    {
      auto p = selectedRows[0];
      cb(p);
      selection->select(p);
      scroll_to_row(p);
    }
  }

  void FileBrowser::setFolder(Glib::RefPtr<Gio::File> s)
  {
    m_currentFolder = s;
    m_store->clear();

    auto e = s->enumerate_children();

    while(auto c = e->next_file())
    {
      auto ct = c->get_content_type();
      auto mt = Gio::content_type_get_mime_type(ct);

      if(ct.find("audio/") == 0 || mt == "inode/directory")
      {
        Gtk::TreeModel::Row row = *(m_store->append());
        row[c_columns.col_name] = c->get_display_name();
        auto n = c->get_name();
        auto c = s->get_child(n);
        row[c_columns.col_data] = c;
      }
    }

    get_selection()->select(Gtk::TreeModel::Path("0"));
  }
}
