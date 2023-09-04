#pragma once

#include <memory>

namespace Dsp
{
  namespace Api
  {
    namespace Control
    {
      class Api;
    }
    namespace Display
    {
      class Api;
    }
    namespace Realtime
    {
      class Api;
    }
  }

  class Dsp
  {
   public:
    Dsp();
    ~Dsp();

    Api::Control::Api &getControlApi() const;
    Api::Realtime::Api &getRealtimeApi() const;
    Api::Display::Api &getDisplayApi() const;

   private:
    std::unique_ptr<Api::Control::Api> m_api;
    std::unique_ptr<Api::Display::Api> m_displayApi;
    std::unique_ptr<Api::Realtime::Api> m_realtimeApi;
  };
}