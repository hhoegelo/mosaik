#pragma once

#include <tools/ReactiveVar.h>
#include <core/Types.h>

namespace Core::Api
{
  class Computation;
}

namespace Ui
{
  class SharedState
  {
   public:
    SharedState();

    enum class Toolboxes
    {
      Global,
      Tile,
      Waveform
    };

    void select(Toolboxes t);
    Toolboxes getSelectedToolbox() const;

    // waveform properties
    double getWaveformZoom() const;
    void incWaveformZoom(int inc);

    Core::FramePos getWaveformScroll() const;
    void incWaveformScroll(int inc);
    void fixWaveformZoom(double d);
    void fixWaveformScroll(Core::FramePos d);

   private:
    Tools::ReactiveVar<Toolboxes> m_selection { Toolboxes::Global };

    struct WaveformProps
    {
      Tools::ReactiveVar<double> zoom { 1.0 };
      Tools::ReactiveVar<Core::FramePos> scroll { 0 };
    } m_waveformProps;
  };
}