#pragma once

#include <core/Types.h>
#include <ui/Types.h>
#include <tools/ReactiveVar.h>

namespace Ui::Touch
{
  class Interface
  {
   public:
    class Waveform
    {
     public:
      virtual ~Waveform() = default;
      virtual void incScroll(int inc) = 0;
      virtual void incZoom(int inc) = 0;
      [[nodiscard]] virtual double getFramesPerPixel() const = 0;
    };

    class FileBrowser
    {
     public:
      virtual ~FileBrowser() = default;
      virtual void inc() = 0;
      virtual void dec() = 0;
      virtual void load() = 0;
      virtual void up() = 0;
      virtual void down() = 0;
    };

    virtual ~Interface() = default;

    virtual Waveform &getWaveform() const = 0;
    virtual FileBrowser &getFileBrowser() const = 0;
    
    [[nodiscard]] virtual ::Ui::Toolboxes getSelectedToolbox() const = 0;
  };
}