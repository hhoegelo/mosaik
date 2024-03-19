#include "SharedState.h"
#include "core/api/Interface.h"

namespace Ui
{
  SharedState::SharedState()
  {
    m_selection.emit(Toolboxes::Global);

    m_waveformProps.scroll.emit(0);
    m_waveformProps.zoom.emit(1);
  }

  void SharedState::select(SharedState::Toolboxes t)
  {
    m_selection.emit(t);

    m_waveformProps.scroll.emit(0);
    m_waveformProps.zoom.emit(1);
  }

  Tools::Signals::Connection SharedState::connectSelectedToolbox(const std::function<void(Toolboxes)> &cb)
  {
    return m_selection.connectWithInit(cb);
  }

  double SharedState::getWaveformZoom(Core::Api::Computation *c) const
  {
    return Core::Api::connect(c, m_waveformProps.zoom);
  }

  void SharedState::incWaveformZoom(int inc)
  {
    m_waveformProps.zoom.emit(getWaveformZoom(nullptr) + inc / 50.0);
  }

  double SharedState::getWaveformScroll(Core::Api::Computation *c) const
  {
    return Core::Api::connect(c, m_waveformProps.scroll);
  }

  void SharedState::incWaveformScroll(int inc)
  {
    m_waveformProps.scroll.emit(getWaveformScroll(nullptr) + inc * getWaveformZoom(nullptr));
  }

  void SharedState::fixWaveformScroll(double d)
  {
    m_waveformProps.scroll.setCache(d);
  }

  void SharedState::fixWaveformZoom(double d)
  {
    m_waveformProps.zoom.setCache(d);
  }

}