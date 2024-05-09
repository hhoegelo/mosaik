#pragma once

#include <gtkmm/drawingarea.h>
#include "core/Types.h"
#include "tools/ReactiveVar.h"
#include "ui/touch-ui/Interface.h"

namespace Tools
{
  class Computations;
}

namespace Core::Api
{
  class Interface;
}

namespace Ui
{
  namespace Touch
  {
    class Waveform : public Gtk::DrawingArea, public WaveformInterface
    {
     public:
      explicit Waveform(Core::Api::Interface &core);

      void incZoom(int inc) override;
      void incScroll(int inc) override;
      double getFramesPerPixel() const override;
      double getZoom() const override;
      Core::FramePos getScroll() const override;

     private:
      bool drawWave(const Cairo::RefPtr<Cairo::Context> &ctx);
      double getSanitizedZoom() const;
      Core::FramePos getSanitizedScroll() const;

      Core::Api::Interface &m_core;
      std::unique_ptr<Tools::Computations> m_computations;

      Tools::DeferredComputations m_staticComputations;

      Tools::ReactiveVar<Core::FramePos> m_scrollPos;
      Tools::ReactiveVar<double> m_zoom { 1.0 };
    };
  }
}