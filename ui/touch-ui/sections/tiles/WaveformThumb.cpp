#include "WaveformThumb.h"
#include <dsp/api/display/Interface.h>
#include <gtkmm/stylecontext.h>
#include <gtkmm/cssprovider.h>
#include <glibmm/main.h>

namespace Ui::Touch
{
  WaveformThumb::WaveformThumb(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Core::Address address)
      : ObjectBase("WaveformThumb")
  {
    get_style_context()->add_class("waveform");

    Glib::MainContext::get_default()->signal_timeout().connect(
        [this]
        {
          queue_draw();
          return true;
        },
        10);

    signal_draw().connect(
        [this, address, &core, &dsp](const Cairo::RefPtr<Cairo::Context>& ctx)
        {
          auto w = get_width();
          auto h = get_height();

          auto isMuted = std::get<bool>(core.getParameter(address, Core::ParameterId::Mute));
          get_style_context()->render_background(ctx, 0, 0, w, h);

          ctx->set_line_width(1);

          auto normalColor = get_style_context()->get_color(Gtk::StateFlags::STATE_FLAG_NORMAL);
          auto activeColor = get_style_context()->get_color(Gtk::StateFlags::STATE_FLAG_ACTIVE);
          auto vistedColor = get_style_context()->get_color(Gtk::StateFlags::STATE_FLAG_VISITED);

          auto samples = core.getSamples(address);
          auto adv = std::max<double>(1, samples.get()->size() / static_cast<double>(w));
          auto frame = 0.0;

          auto pos = dsp.getPosition(address);

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

            if(!isMuted && frame >= pos && pos >= 0)
            {
              ctx->set_source_rgb(activeColor.get_red(), activeColor.get_green(), activeColor.get_blue());
            }
            else if(!isMuted)
            {
              ctx->set_source_rgb(vistedColor.get_red(), vistedColor.get_green(), vistedColor.get_blue());
            }
            else
            {
              ctx->set_source_rgb(normalColor.get_red(), normalColor.get_green(), normalColor.get_blue());
            }

            ctx->move_to(i, h / 2 + v * h / 2);
            ctx->line_to(i, h / 2 - v * h / 2);
            ctx->stroke();
            frame += adv;
          }

          return true;
        });
  }
}