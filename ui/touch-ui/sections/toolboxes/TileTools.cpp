#include "TileTools.h"
#include "core/api/Interface.h"
#include "core/Types.h"
#include "FileBrowser.h"

#include <gtkmm/filechooserwidget.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/levelbar.h>

#include <gdkmm/seat.h>
#include <gtkmm/treeview.h>
#include <gtkmm/listbox.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>

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

  Gtk::Widget *findChildWidget(Gtk::Widget *p, const char *name)
  {
    auto n = p->get_name();
    if(n == name)
      return p;

    if(auto c = dynamic_cast<Gtk::Container *>(p))
      for(auto child : c->get_children())
        if(auto found = findChildWidget(child, name))
          return found;

    return nullptr;
  }

  TileTools::TileTools(Core::Api::Interface &core)
      : Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL)
      , m_core(core)
  {
    m_fileBrowser = Gtk::manage(new FileBrowser(core));
    auto fileBrowserScroll = Gtk::manage(new Gtk::ScrolledWindow());
    fileBrowserScroll->add(*m_fileBrowser);
    m_fileBrowser->get_style_context()->add_class("file-browser");
    pack_start(*fileBrowserScroll);

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

  FileBrowserInterface &TileTools::getFileBrowser() const
  {
    return *m_fileBrowser;
  }
}
