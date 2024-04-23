#pragma once

#include "ui/touch-ui/Interface.h"
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class FileBrowser : public Gtk::TreeView, public FileBrowserInterface
  {
   public:
    FileBrowser(Core::Api::Interface &core);

    void up() override;
    void down() override;
    void inc() override;
    void dec() override;
    void load() override;
    void prelisten() override;

   private:
    void navigate(const std::function<void(Gtk::TreePath &)> &cb);
    void setFolder(Glib::RefPtr<Gio::File> s);

    Core::Api::Interface &m_core;
    Glib::RefPtr<Gtk::ListStore> m_store;
    Glib::RefPtr<Gio::File> m_currentFolder;
  };
}