#pragma once

#include <memory>

namespace Dsp
{
  namespace Api
  {
    namespace Control
    {
      class Interface;
      class Mosaik;
    }
    namespace Display
    {
      class Interface;
      class Mosaik;
    }
    namespace Realtime
    {
      class Interface;
      class Mosaik;
    }
  }

  class Dsp
  {
   public:
    Dsp();
    ~Dsp();

    Api::Control::Interface &getControlApi() const;
    Api::Realtime::Interface &getRealtimeApi() const;
    Api::Display::Interface &getDisplayApi() const;

   private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;

    std::unique_ptr<Api::Control::Interface> m_controlApi;
    std::unique_ptr<Api::Display::Interface> m_displayApi;
    std::unique_ptr<Api::Realtime::Interface> m_realtimeApi;

    friend class Api::Control::Mosaik;
    friend class Api::Display::Mosaik;
    friend class Api::Realtime::Mosaik;
  };
}