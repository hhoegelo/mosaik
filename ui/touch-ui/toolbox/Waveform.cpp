#include "Waveform.h"
#include <ui/SharedState.h>
#include <core/api/Interface.h>
#include <tools/ReactiveVar.h>

namespace Ui::Touch
{
  Waveform::Waveform(Ui::SharedState& sharedUiState, Core::Api::Interface& core)
      : m_sharedUiState(sharedUiState)
      , m_core(core)
  {
    auto styles = get_style_context();
    styles->add_class("waveform");
    set_size_request(0, 250);
    signal_draw().connect([this](const Cairo::RefPtr<Cairo::Context>& ctx) { return drawWave(ctx); });
  }

  bool Waveform::drawWave(const Cairo::RefPtr<Cairo::Context>& ctx)
  {
    m_computations = std::make_unique<Tools::DeferredComputations>(Glib::MainContext::get_default());
    m_computations->add(
        [this, ctx, init = true]() mutable
        {
          if(!std::exchange(init, false))
          {
            this->queue_draw();
            return;
          }

          ctx->set_line_width(1);

          auto tileId = m_core.getSelectedTiles().front();
          auto samples = m_core.getSamples(tileId);
          auto numSamples = static_cast<double>(samples.get()->size());
          auto w = get_width();
          auto h = get_height();

          auto minZoom = 1.0;
          auto maxZoom = numSamples / w;
          auto zoom = m_sharedUiState.getWaveformZoom();

          zoom = std::min(zoom, maxZoom);
          zoom = std::max(zoom, minZoom);

          m_sharedUiState.fixWaveformZoom(zoom);

          auto adv = std::max<double>(1, numSamples / static_cast<double>(zoom * w));

          auto minScroll = 0.0;
          auto maxScroll = numSamples - (w * adv);
          auto scroll = static_cast<double>(m_sharedUiState.getWaveformScroll());

          scroll = std::min(scroll, maxScroll);
          scroll = std::max(scroll, minScroll);

          m_sharedUiState.fixWaveformScroll(scroll);

          auto frame = scroll;

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
        });
    return true;
  }

}