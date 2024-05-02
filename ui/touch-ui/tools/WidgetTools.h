#pragma once

#include <gtkmm/widget.h>
#include <gtkmm/container.h>

namespace Ui::Touch
{
  template <typename T> T *findChildWidget(const Gtk::Widget *p)
  {
    auto pThis = const_cast<Gtk::Widget *>(p);

    if(auto found = dynamic_cast<T *>(pThis))
      return found;

    if(auto c = dynamic_cast<const Gtk::Container *>(pThis))
      for(auto child : c->get_children())
        if(auto found = findChildWidget<T>(child))
          return found;

    return nullptr;
  }

  Gtk::Widget *findChildWidget(const Gtk::Widget *p, const char *name);
}