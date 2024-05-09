#include "WidgetTools.h"

Gtk::Widget *Ui::Touch::findChildWidget(const Gtk::Widget *p, const char *name)
{
  auto pThis = const_cast<Gtk::Widget *>(p);

  if(auto b = reinterpret_cast<const Gtk::Buildable *>(p))
  {
    if(b->get_name() == name)
      return pThis;
  }

  if(pThis->get_name() == name)
    return pThis;

  if(auto c = dynamic_cast<const Gtk::Container *>(pThis))
    for(auto child : c->get_children())
      if(auto found = findChildWidget(child, name))
        return found;

  return nullptr;
}
