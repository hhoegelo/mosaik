#include "Toolbox.h"
#include "ui/touch-ui/Interface.h"
#include <gtkmm/label.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/adjustment.h>

using namespace std::chrono_literals;

namespace Ui::Touch
{

  Toolbox::Toolbox(ToolboxesInterface &toolboxes, Ui::Toolbox tool, Gtk::Widget *maximized)
  {
    set_orientation(Gtk::ORIENTATION_VERTICAL);

    get_style_context()->add_class("toolbox");

    set_size_request(540, -1);

    pack_start(*maximized, Gtk::PACK_EXPAND_WIDGET);

    auto animate = [this](auto w, auto t)
    {
      auto startTime = std::chrono::system_clock::now();
      auto startPos = w->get_vadjustment()->get_value();

      add_tick_callback(
          [this, w, t, startTime, startPos](const Glib::RefPtr<Gdk::FrameClock> &clock)
          {
            constexpr auto scrollTime = 500000000ns;
            auto now = std::chrono::system_clock::now();
            auto diff = now - startTime;
            auto ratio = std::min<double>(1.0, static_cast<double>(diff.count()) / scrollTime.count());
            double eased_progress = -0.5 * (std::cos(M_PI * ratio) - 1);
            auto vadjustment = w->get_vadjustment();
            vadjustment->set_value(startPos + eased_progress * (t - startPos));
            w->set_vadjustment(vadjustment);
            return ratio < 1;
          });
    };

    m_computations.add(
        [this, &toolboxes, tool, animate]
        {
          if(toolboxes.getSelectedToolbox() == tool)
          {
            get_style_context()->add_class("selected");

            Gtk::Container *parent = get_parent();

            while(parent && !dynamic_cast<Gtk::ScrolledWindow *>(parent))
              parent = parent->get_parent();

            if(auto w = dynamic_cast<Gtk::ScrolledWindow *>(parent))
            {
              auto vadjustment = w->get_vadjustment();
              auto rect = get_allocation();
              animate(w, rect.get_y() - (vadjustment->get_page_size() - rect.get_height()) / 2);
            }
          }
          else
          {
            get_style_context()->remove_class("selected");
          }
        });
  }
}
