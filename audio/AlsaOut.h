#pragma once

#include <string>
#include <future>

namespace Dsp::Api::Realtime
{
  class Interface;
}

namespace Audio
{
  class AlsaOut
  {
   public:
    AlsaOut(Dsp::Api::Realtime::Interface &dsp, const std::string &device, int bits, int channels);
    ~AlsaOut();

   private:
    template <typename Sample>
    void audioThread(Dsp::Api::Realtime::Interface &dsp, const std::string &device, int channels);

    template <typename Sample, typename AlsaFrame, int numChannels>
    void audioThread(Dsp::Api::Realtime::Interface &dsp, const std::string &device);

    std::atomic_bool m_quit {};
    std::future<void> m_audioThread;
  };
}