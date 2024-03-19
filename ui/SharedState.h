#pragma once

#include <tools/SignalSlot.h>
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
    [[nodiscard]] Tools::Signals::Connection connectSelectedToolbox(const std::function<void(Toolboxes)> &cb);

    // waveform properties
    double getWaveformZoom(Core::Api::Computation *c) const;
    void incWaveformZoom(int inc);

    double getWaveformScroll(Core::Api::Computation *c) const;
    void incWaveformScroll(int inc);

    void fixWaveformZoom(double d);

    void fixWaveformScroll(double d);

   private:
    Tools::Signals::Signal<Toolboxes> m_selection;

    struct WaveformProps
    {
      Tools::Signals::Signal<double> zoom;
      Tools::Signals::Signal<Core::FramePos> scroll;
    } m_waveformProps;
  };
}