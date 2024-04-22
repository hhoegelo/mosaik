
#include "WaveformThumb.h"
#include <gtkmm/stylecontext.h>
#include <gtkmm/cssprovider.h>

namespace Ui::Touch
{
  WaveformThumb::WaveformThumb(Core::Api::Interface& core, Core::TileId tileId)
      : ObjectBase("WaveformThumb")
      , m_foo(*this, "foo", 12)
  {
    auto styles = get_style_context();
    styles->add_class("waveform");

    set_size_request(50, 50);

    get_style_context()->signal_changed().connect(
        [this]
        {
          guint i = m_foo.get_value();
          printf("I = %d \n", i);
        });

    signal_draw().connect(
        [this, tileId, &core](const Cairo::RefPtr<Cairo::Context>& ctx)
        {
          ctx->set_line_width(1);

          auto samples = core.getSamples(tileId);
          auto w = get_width();
          auto h = get_height();
          auto adv = std::max<double>(1, samples.get()->size() / static_cast<double>(w));
          auto frame = 0.0;

          for(size_t i = 0; i < w; i++)
          {
            float v = 0;

            for(size_t a = 0; a < static_cast<size_t>(adv); a++)
            {
              auto idx = static_cast<size_t>(std::round(frame + a));
              if(idx < samples->size())
              {
                v = std::max(v, std::abs(std::max(samples->data()[idx].left, samples->data()[idx].right)));
              }
            }

            ctx->move_to(i, h / 2 + v * h / 2);
            ctx->line_to(i, h / 2 - v * h / 2);
            frame += adv;
          }

          ctx->stroke();
          return true;
        });

    add_events(Gdk::EventMask::BUTTON_PRESS_MASK | Gdk::EventMask::POINTER_MOTION_MASK);

    signal_button_press_event().connect(
        [&core, tileId](GdkEventButton*)
        {
          core.setParameter(tileId, Core::ParameterId::Selected, true);
          return false;
        });
  }
}