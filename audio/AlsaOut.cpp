#include "AlsaOut.h"
#include <dsp/MidiEvent.h>
#include <dsp/api/realtime/Interface.h>
#include <alsa/asoundlib.h>

#include <glibmm.h>
#include <iostream>
#include <numeric>

#define checkAlsa(A)                                                                                                   \
  if(auto res = A)                                                                                                     \
  std::cerr << "Alsa Audio Error: " << #A << " throws error: " << snd_strerror(res) << std::endl

namespace
{
  constexpr auto c_numPeriods = 8;
  constexpr auto c_framesPerPeriod = 256;
  constexpr float c_maxSample = 1.0f;
  constexpr float c_minSample = -1.0f;

  struct S16
  {
    static constexpr snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S16;
    static constexpr float FLOAT_TO_INT = static_cast<float>((1 << 14) - 1);
    using Sample = int16_t;

    inline static Sample fromFloat(float f)
    {
      return static_cast<Sample>(std::clamp(f, c_minSample, c_maxSample) * FLOAT_TO_INT);
    }
  };

  struct S24
  {
    static constexpr snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S24_3LE;
    static constexpr float FLOAT_TO_INT = static_cast<float>((1 << 22) - 1);
    struct Sample
    {
      uint8_t a, b, c;
    };

    inline static Sample fromFloat(float f)
    {
      auto q = static_cast<int32_t>(std::clamp(f, c_minSample, c_maxSample) * FLOAT_TO_INT);
      uint8_t a = q & 0xFF;
      uint8_t b = (q >> 8) & 0xFF;
      uint8_t c = (q >> 16) & 0xFF;

      return { a, b, c };
    }
  };

  struct S32
  {
    static constexpr snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S32;
    static constexpr float FLOAT_TO_INT = static_cast<float>((1 << 30) - 1);
    using Sample = int32_t;

    inline static Sample fromFloat(float f)
    {
      return static_cast<Sample>(std::clamp(f, c_minSample, c_maxSample) * FLOAT_TO_INT);
    }
  };

  template <typename SampleTrait> struct Frame2Ch
  {
    Frame2Ch() = default;

    explicit Frame2Ch(const Dsp::OutFrame &in, bool switchMainAndPre)
        : left { SampleTrait::fromFloat(in.main.left + in.pre.left) }
        , right { SampleTrait::fromFloat(in.main.right + in.pre.right) }
    {
    }

    SampleTrait::Sample left;
    SampleTrait::Sample right;
  };

  template <typename SampleTrait> struct Frame4Ch
  {
    Frame4Ch() = default;

    explicit Frame4Ch(const Dsp::OutFrame &in, bool switchMainAndPre)
        : mainLeft { SampleTrait::fromFloat(in.main.left) }
        , mainRight { SampleTrait::fromFloat(in.main.right) }
        , preLeft { SampleTrait::fromFloat(in.pre.left) }
        , preRight { SampleTrait::fromFloat(in.pre.right) }
    {
      if(switchMainAndPre)
      {
        std::swap(mainLeft, preLeft);
        std::swap(mainRight, preRight);
      }
    }

    SampleTrait::Sample mainLeft;
    SampleTrait::Sample mainRight;
    SampleTrait::Sample preLeft;
    SampleTrait::Sample preRight;
  };
}

namespace Audio
{
  AlsaOut::AlsaOut(Dsp::Api::Realtime::Interface &dsp, const std::string &device, int bits, int channels,
                   bool switchMainAndPre)
  {
    if(!device.empty())
    {
      m_audioThread = std::async(std::launch::async,
                                 [this, &dsp, device, bits, channels, switchMainAndPre]
                                 {
                                   if(bits == 16)
                                     audioThread<S16>(dsp, device, channels, switchMainAndPre);
                                   else if(bits == 24)
                                     audioThread<S24>(dsp, device, channels, switchMainAndPre);
                                   else
                                     audioThread<S32>(dsp, device, channels, switchMainAndPre);
                                 });
    }

    m_timer = Glib::MainContext::get_default()->signal_timeout().connect_seconds(
        [this]
        {
          auto average = std::accumulate(m_cpuUsage.begin(), m_cpuUsage.end(), 0.0) / m_cpuUsage.size();
          std::cout << "AudioThread CPU Usage: " << 100.0 * average << "%" << std::endl;
          return true;
        },
        2);
  }

  AlsaOut::~AlsaOut()
  {
    m_timer.disconnect();
    m_quit = true;

    if(m_audioThread.valid())
      m_audioThread.wait();
  }

  template <typename SampleTrait>
  void AlsaOut::audioThread(Dsp::Api::Realtime::Interface &dsp, const std::string &device, int channels,
                            bool switchMainAndPre)
  {
    if(channels == 2)
      audioThread<SampleTrait, Frame2Ch<SampleTrait>, 2>(dsp, device, switchMainAndPre);
    else if(channels == 4)
      audioThread<SampleTrait, Frame4Ch<SampleTrait>, 4>(dsp, device, switchMainAndPre);
    else
      throw std::invalid_argument("num channels not supported");
  }

  template <typename SampleTrait, typename AlsaFrame, int numChannels>
  void AlsaOut::audioThread(Dsp::Api::Realtime::Interface &dsp, const std::string &device, bool switchMainAndPre)
  {
    int preferedCore = 1;
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(preferedCore, &set);

    if(sched_setaffinity(0, sizeof(cpu_set_t), &set) < 0)
      std::cerr << "could not set thread affinity" << std::endl;

    sched_param p { sched_get_priority_max(SCHED_FIFO) };
    if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &p))
      std::cerr << "set thread prio failed. Consider 'sudo setcap 'cap_sys_nice=eip' <application>'" << std::endl;

    snd_pcm_t *pcm = nullptr;
    checkAlsa(snd_pcm_open(&pcm, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0));

    snd_pcm_hw_params_t *hwparams = nullptr;
    snd_pcm_hw_params_alloca(&hwparams);

    snd_pcm_hw_params_any(pcm, hwparams);
    checkAlsa(snd_pcm_hw_params_set_access(pcm, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED));
    checkAlsa(snd_pcm_hw_params_set_format(pcm, hwparams, SampleTrait::FORMAT));

    checkAlsa(snd_pcm_hw_params_set_channels(pcm, hwparams, numChannels));
    checkAlsa(snd_pcm_hw_params_set_rate(pcm, hwparams, SAMPLERATE, 0));

    checkAlsa(snd_pcm_hw_params_set_periods(pcm, hwparams, c_numPeriods, 0));
    checkAlsa(snd_pcm_hw_params_set_period_size(pcm, hwparams, c_framesPerPeriod, 0));

    checkAlsa(snd_pcm_hw_params(pcm, hwparams));
    checkAlsa(snd_pcm_prepare(pcm));

    AlsaFrame buffer[c_framesPerPeriod] {};

    for(int i = 0; i < c_numPeriods; i++)
      snd_pcm_writei(pcm, buffer, c_framesPerPeriod);

    snd_pcm_start(pcm);

    Dsp::OutFrame samples[c_framesPerPeriod] = {};

    while(!m_quit)
    {
      auto startDSP = g_get_monotonic_time();

      dsp.doAudio(samples, c_framesPerPeriod, [](const Dsp::MidiEvent &) {});
      std::transform(samples, samples + c_framesPerPeriod, buffer,
                     [&switchMainAndPre](const Dsp::OutFrame &in) { return AlsaFrame(in, switchMainAndPre); });
      auto endDSP = g_get_monotonic_time();
      auto diff = endDSP - startDSP;

      m_cpuUsage[m_cpuUsageWriteHead % m_cpuUsage.size()]
          = 1.0 * SAMPLERATE * diff / (std::micro::den * c_framesPerPeriod);
      m_cpuUsageWriteHead++;

      auto res = snd_pcm_writei(pcm, buffer, c_framesPerPeriod);

      if(res != c_framesPerPeriod)
      {
        printf("Buffer underrun\n");
        snd_pcm_recover(pcm, res, true);
      }
    }

    snd_pcm_close(pcm);
  }

}