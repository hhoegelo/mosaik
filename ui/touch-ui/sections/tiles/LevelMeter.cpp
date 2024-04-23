#include "LevelMeter.h"

namespace Ui::Touch
{
  LevelMeter::LevelMeter(const std::string &classes, const std::function<float()> &cb, float decay)
  {
    get_style_context()->add_class("level-meter");
    get_style_context()->add_class(classes);

    signal_draw().connect(
        [this](const Cairo::RefPtr<Cairo::Context> &ctx)
        {
          auto w = get_width();
          auto h = get_height();
          get_style_context()->render_background(ctx, 0, 0, w, h);

          auto height = static_cast<double>(h);
          ctx->begin_new_path();
          auto c = get_style_context()->get_color();
          ctx->set_source_rgba(c.get_red(), c.get_green(), c.get_blue(), c.get_alpha());
          ctx->set_line_width(1);
          ctx->move_to(1, height);
          ctx->line_to(1, (1.0f - m_currentValue) * height);
          ctx->stroke();
          return true;
        });

    m_computations.add(
        [this, decay, cb]
        {
          m_currentValue = std::max(cb(), m_currentValue * decay);
          queue_draw();
        });
  }
  void LevelMeter::on_size_allocate(Gtk::Allocation &allocation)
  {
    allocation.set_width(2);
    Widget::on_size_allocate(allocation);
  }
}
