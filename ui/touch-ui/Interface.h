#pragma once

#include <core/Types.h>
#include <ui/Types.h>
#include <tools/ReactiveVar.h>

namespace Ui::Touch
{
  class Interface
  {
   public:
    virtual ~Interface() = default;
    virtual void incWaveformScroll(int inc) = 0;
    virtual void incWaveformZoom(int inc) = 0;
    [[nodiscard]] virtual ::Ui::Toolboxes getSelectedToolbox() const = 0;
    virtual double getWaveformFramesPerPixel() const = 0;
  };
}