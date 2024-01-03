#pragma once

#include <string>
#include <future>

namespace Dsp
{
  namespace Api
  {
    namespace Realtime
    {
      class Interface;
    }
  }
}

namespace Audio
{

  class AlsaOut
  {
   public:
    AlsaOut(Dsp::Api::Realtime::Interface &dsp, const std::string &device);
    ~AlsaOut();

   private:
    std::atomic_bool m_quit;
    std::future<void> m_audioThread;
  };
}