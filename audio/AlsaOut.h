#pragma once

#include <sigc++/connection.h>
#include <string>
#include <future>
#include <array>

namespace Dsp::Api::Realtime
{
  class Interface;
}

namespace Audio
{
  class AlsaOut
  {
   public:
    AlsaOut(Dsp::Api::Realtime::Interface &dsp, const std::string &device, int bits, int channels,
            bool switchMainAndPre);
    ~AlsaOut();

   private:
    template <typename Sample>
    void audioThread(Dsp::Api::Realtime::Interface &dsp, const std::string &device, int channels,
                     bool switchMainAndPre);

    template <typename Sample, typename AlsaFrame, int numChannels>
    void audioThread(Dsp::Api::Realtime::Interface &dsp, const std::string &device, bool switchMainAndPre);

    std::atomic_bool m_quit {};
    std::future<void> m_audioThread;

    std::array<double, 128> m_cpuUsage { 0.0 };
    uint32_t m_cpuUsageWriteHead = 0;

    sigc::connection m_timer;
  };
}