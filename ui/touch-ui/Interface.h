#pragma once

#include <core/Types.h>
#include <ui/Types.h>
#include <tools/ReactiveVar.h>

namespace Ui::Touch
{
  class WaveformInterface
  {
   public:
    virtual ~WaveformInterface() = default;
    virtual void incScroll(int inc) = 0;
    virtual void incZoom(int inc) = 0;
    [[nodiscard]] virtual double getZoom() const = 0;
    [[nodiscard]] virtual Core::FramePos getScroll() const = 0;
    [[nodiscard]] virtual double getFramesPerPixel() const = 0;
  };

  class FileBrowserInterface
  {
   public:
    virtual ~FileBrowserInterface() = default;
    virtual void inc() = 0;
    virtual void dec() = 0;
    virtual void load() = 0;
    virtual void up() = 0;
    virtual void down() = 0;
  };

  class ToolboxesInterface
  {
   public:
    [[nodiscard]] virtual WaveformInterface &getWaveform() const = 0;
    [[nodiscard]] virtual FileBrowserInterface &getFileBrowser() const = 0;
    [[nodiscard]] virtual Ui::Toolbox getSelectedToolbox() const = 0;
    virtual void selectToolbox(Ui::Toolbox t) = 0;
  };

  class Interface
  {
   public:
    virtual ~Interface() = default;
    virtual ToolboxesInterface &getToolboxes() const = 0;

    virtual void setColorAdjustmentColor(int r, int g, int b) = 0;
  };
}