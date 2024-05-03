

#include "Erp.h"

namespace Ui::Midi
{

  Erp::Erp()
  {
    set_size_request(50, 50);
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK);
  }
  bool Erp::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
  {
    Gtk::Allocation allocation = get_allocation();
    const double width = allocation.get_width();
    const double height = allocation.get_height();
    const double radius = std::min(width, height) / 2;

    cr->save();

    get_style_context()->render_background(cr, 0, 0, width, height);

    cr->set_line_width(1.0);
    cr->set_source_rgb(0, 0, 0);
    cr->arc(width / 2, height / 2, radius - 1, 0.0, 2 * M_PI);
    cr->stroke();

    for(int i = 0; i < 360; i += 72)
    {
      auto d = 2 * M_PI * (m_degree + i) / 360.0;

      cr->set_line_width(4.0);
      cr->move_to(width / 2, height / 2);
      cr->line_to(width / 2 + cos(d) * radius, height / 2 + sin(d) * radius);
      cr->stroke();
    }
    cr->restore();

    return true;
  }

  bool Erp::on_button_press_event(GdkEventButton *event)
  {
    if(event->button == GDK_BUTTON_PRIMARY)
      m_lastMousePos = Point { event->x, event->y };

    if(event->button > 1)
      m_down.emit();
    return true;
  }
  bool Erp::on_button_release_event(GdkEventButton *event)
  {
    if(event->button == GDK_BUTTON_PRIMARY)
      m_lastMousePos.reset();

    if(event->button > 1)
      m_up.emit();
    
    return true;
  }
  bool Erp::on_motion_notify_event(GdkEventMotion *event)
  {
    if(m_lastMousePos)
    {
      auto xDiff = event->x - std::get<0>(m_lastMousePos.value());
      auto yDiff = std::get<1>(m_lastMousePos.value()) - event->y;

      auto change = std::abs(xDiff) > std::abs(yDiff) ? xDiff : yDiff;
      m_degree += change;
      m_turn.emit(change);
      queue_draw();

      m_lastMousePos = Point { event->x, event->y };
    }
    return true;
  }

  sigc::connection Erp::connect(const std::function<void(int)> &cb)
  {
    return m_turn.connect(cb);
  }

  sigc::connection Erp::down(const std::function<void()> &cb)
  {
    return m_down.connect(cb);
  }

  sigc::connection Erp::up(const std::function<void()> &cb)
  {
    return m_up.connect(cb);
  }
}