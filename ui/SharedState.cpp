#include "SharedState.h"
#include "core/api/Interface.h"

namespace Ui
{
  SharedState::SharedState()
  {
    m_selection = Toolboxes::Global;
    m_waveformProps.scroll = 0;
    m_waveformProps.zoom = 1;
  }

  void SharedState::select(SharedState::Toolboxes t)
  {
    m_selection = t;
    m_waveformProps.scroll = 0;
    m_waveformProps.zoom = 1;
  }

  SharedState::Toolboxes SharedState::getSelectedToolbox() const
  {
    return m_selection;
  }

  double SharedState::getWaveformZoom() const
  {
    return m_waveformProps.zoom;
  }

  void SharedState::incWaveformZoom(int inc)
  {
    //m_waveformProps.zoom.emit(getWaveformZoom(nullptr) + inc / 50.0);
  }

  Core::FramePos SharedState::getWaveformScroll() const
  {
    return m_waveformProps.scroll;
  }

  void SharedState::incWaveformScroll(int inc)
  {
    m_waveformProps.scroll = m_waveformProps.scroll + inc * getWaveformZoom();
  }

  void SharedState::fixWaveformScroll(Core::FramePos d)
  {
    m_waveformProps.scroll = d;
  }

  void SharedState::fixWaveformZoom(double d)
  {
    m_waveformProps.zoom = d;
  }

}