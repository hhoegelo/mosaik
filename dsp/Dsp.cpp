#include "Dsp.h"
#include "api/control/Api.h"
#include "api/display/Api.h"
#include "api/realtime/Api.h"

namespace Dsp
{
  Dsp::Dsp()
  {
  }

  Dsp::~Dsp() = default;

  Api::Control::Api &Dsp::getControlApi() const
  {
    return *m_api;
  }

  Api::Realtime::Api &Dsp::getRealtimeApi() const
  {
    return *m_realtimeApi;
  }

  Api::Display::Api &Dsp::getDisplayApi() const
  {
    return *m_displayApi;
  }
}