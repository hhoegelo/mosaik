#include "Waveform.h"
#include "core/api/Interface.h"
#include "tools/ReactiveVar.h"

namespace Ui::Touch
{
  Waveform::Waveform(Core::Api::Interface& core)
      : m_core(core)
  {
    auto styles = get_style_context();
    styles->add_class("waveform");
    set_size_request(0, 150);

    m_staticComputations.add(
        [this]
        {
          auto tileId = m_core.getSelectedTiles().front();
          auto samples = *m_core.getSamples(tileId);
          m_scrollPos = 0;
          m_zoom = 1.0;
        });

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
          auto samples = *m_core.getSamples(tileId);
          auto numFrames = static_cast<double>(samples.size());
          auto w = static_cast<double>(get_width());
          auto h = static_cast<double>(get_height());

          auto zoom = getSanitizedZoom();

          auto numPixels = static_cast<double>(zoom * w);
          auto numFramesPerPixel = std::max<double>(1, numFrames / numPixels);
          auto scrollPos = static_cast<double>(getSanitizedScroll());

          auto reverse = std::get<bool>(m_core.getParameter(tileId, Core::ParameterId::Reverse));

          auto triggerPos = std::get<Core::FramePos>(m_core.getParameter(tileId, Core::ParameterId::TriggerFrame));

          if(reverse)
          {
            triggerPos = samples.size() - triggerPos;
          }

          auto fadeInPos = std::get<Core::FramePos>(m_core.getParameter(tileId, Core::ParameterId::EnvelopeFadeInPos));
          auto fadedInPos
              = std::get<Core::FramePos>(m_core.getParameter(tileId, Core::ParameterId::EnvelopeFadedInPos));
          auto fadeOutPos
              = std::get<Core::FramePos>(m_core.getParameter(tileId, Core::ParameterId::EnvelopeFadeOutPos));
          auto fadedOutPos
              = std::get<Core::FramePos>(m_core.getParameter(tileId, Core::ParameterId::EnvelopeFadedOutPos));

          for(size_t i = 0; i < get_width(); i++)
          {
            float v = 0;
            ctx->begin_new_path();

            auto frame = scrollPos + std::round(static_cast<double>(i) * numFramesPerPixel);
            auto gray = 1.0;
            if(frame < fadeInPos)
              gray = 1.0;
            else if(frame < fadedInPos)
              gray = 1.0 - (frame - fadeInPos) / std::max<Core::FramePos>(1, fadedInPos - fadeInPos);
            else if(frame < fadeOutPos)
              gray = 0.0;
            else if(frame < fadedOutPos)
              gray = ((frame - fadeOutPos) / std::max<Core::FramePos>(1, fadedOutPos - fadeOutPos));
            else
              gray = 1.0;

            ctx->set_source_rgb(gray, gray, gray);

            for(size_t a = 0; a < static_cast<size_t>(numFramesPerPixel); a++)
            {
              auto idx = static_cast<size_t>(frame) + a;

              if(idx < samples.size())
              {
                auto i = reverse ? samples.size() - 1 - idx : idx;

                v = std::max(v, std::abs(std::max(samples[i].left, samples[i].right)));
              }
            }

            ctx->move_to(static_cast<double>(i), h / 2 + v * h / 2);
            ctx->line_to(static_cast<double>(i), h / 2 - v * h / 2);
            ctx->stroke();
          }

          ctx->begin_new_path();
          ctx->set_source_rgb(0, 0, 0);
          ctx->move_to((fadeInPos - scrollPos) / numFramesPerPixel, h);
          ctx->line_to((fadedInPos - scrollPos) / numFramesPerPixel, 0);
          ctx->line_to((fadeOutPos - scrollPos) / numFramesPerPixel, 0);
          ctx->line_to((fadedOutPos - scrollPos) / numFramesPerPixel, h);
          ctx->stroke();

          ctx->begin_new_path();
          ctx->set_source_rgb(1.0, 0, 0);
          ctx->move_to((triggerPos - scrollPos) / numFramesPerPixel, h);
          ctx->line_to((triggerPos - scrollPos) / numFramesPerPixel, 0);
          ctx->stroke();
        });
    return true;
  }

  void Waveform::incScroll(int inc)
  {
    auto numFramesPerPixel = getFramesPerPixel();
    m_scrollPos = getSanitizedScroll() + static_cast<Core::FramePos>(inc * numFramesPerPixel);
  }

  void Waveform::incZoom(int inc)
  {
    m_zoom = getSanitizedZoom() + inc / 10.0;
  }

  double Waveform::getSanitizedZoom() const
  {
    auto tileId = m_core.getSelectedTiles().front();
    auto samples = *m_core.getSamples(tileId);
    auto numFrames = static_cast<double>(samples.size());
    auto width = static_cast<double>(get_width());

    double minZoom = 1.0;
    double maxZoom = numFrames / width;

    return std::clamp(m_zoom.get(), minZoom, maxZoom);
  }

  Core::FramePos Waveform::getSanitizedScroll() const
  {
    auto tileId = m_core.getSelectedTiles().front();
    auto samples = *m_core.getSamples(tileId);
    auto numFrames = static_cast<double>(samples.size());
    auto width = static_cast<double>(get_width());

    auto numFramesPerPixel = getFramesPerPixel();
    auto max = numFrames - width * numFramesPerPixel;
    return std::clamp<Core::FramePos>(m_scrollPos.get(), 0, static_cast<Core::FramePos>(max));
  }

  double Waveform::getFramesPerPixel() const
  {
    auto tileId = m_core.getSelectedTiles().front();
    auto samples = *m_core.getSamples(tileId);
    auto numFrames = static_cast<double>(samples.size());
    auto zoom = getSanitizedZoom();
    auto width = static_cast<double>(get_width());
    auto numPixels = static_cast<double>(zoom * width);

    return std::max<double>(1, numFrames / numPixels);
  }

}