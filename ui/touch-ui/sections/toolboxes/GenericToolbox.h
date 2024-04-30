#pragma once

#include "Toolbox.h"
#include <ui/Types.h>
#include <gtkmm/label.h>

namespace Ui::Touch
{

  template <Ui::Toolbox T> class GenericToolbox : public Toolbox
  {
   public:
    GenericToolbox(ToolboxesInterface &toolboxes, Gtk::Widget *maximized = nullptr)
        : Toolbox(toolboxes, T, Ui::ToolboxDefinition<T>::title, buildMinimized(),
                  maximized ? maximized : buildMaximized())
    {
    }

    virtual Gtk::Widget *buildMinimized()
    {
      auto b = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
      for(auto m : Ui::ToolboxDefinition<T>::minimized)
      {
        auto l = Gtk::manage(new Gtk::Label("abc"));
        b->pack_start(*l);
      }
      return b;
    }

    virtual Gtk::Widget *buildMaximized()
    {
      return nullptr;
    }
  };
}