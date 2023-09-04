#include "Core.h"
#include "Api.h"

namespace Core
{
  Core::Core(Dsp::Api::Control::Api &dsp)
  {
  }

  Core::~Core() = default;

  Api &Core::getApi() const
  {
    return *m_api;
  }

}