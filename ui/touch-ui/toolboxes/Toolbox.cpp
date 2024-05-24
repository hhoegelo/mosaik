#include "Toolbox.h"
#include "ui/touch-ui/Interface.h"
#include <gtkmm/label.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/adjustment.h>

namespace Ui::Touch
{

  Toolbox::Toolbox(ToolboxesInterface &toolboxes, Ui::Toolbox tool, Gtk::Widget *maximized)
  {
    set_orientation(Gtk::ORIENTATION_VERTICAL);

    get_style_context()->add_class("toolbox");

    set_size_request(540, -1);

    /* auto eventBox = Gtk::manage(new Gtk::EventBox());
    eventBox->add(*maximized);

    eventBox->signal_button_press_event().connect(
        [tool, &toolboxes](GdkEventButton *)
        {
          toolboxes.selectToolbox(tool);
          return true;
        });*/

    pack_start(*maximized, Gtk::PACK_EXPAND_WIDGET);

    m_computations.add(
        [this, &toolboxes, tool]
        {
          if(toolboxes.getSelectedToolbox() == tool)
          {
            get_style_context()->add_class("selected");

            Gtk::Container *parent = get_parent();

            while(parent && !dynamic_cast<Gtk::ScrolledWindow *>(parent))
            {
              parent = parent->get_parent();
            }

            if(auto w = dynamic_cast<Gtk::ScrolledWindow *>(parent))
            {
              auto vadjustment = w->get_vadjustment();
              auto rect = get_allocation();
              auto min = vadjustment->get_value();
              auto max = min + vadjustment->get_page_size();

              if(rect.get_y() < min)
              {
                vadjustment->set_value(rect.get_y());
                w->set_vadjustment(vadjustment);
              }

              if(rect.get_y() + rect.get_height() > max)
              {
                auto spaceAbove = (vadjustment->get_page_size() - rect.get_height());
                vadjustment->set_value(rect.get_y() - spaceAbove);
                w->set_vadjustment(vadjustment);
              }
            }
          }
          else
          {
            get_style_context()->remove_class("selected");
          }
        });
  }
}
