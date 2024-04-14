#include "AlsaOut.h"
#include <dsp/MidiEvent.h>
#include <dsp/api/realtime/Interface.h>
#include <alsa/asoundlib.h>

#include <iostream>

#define checkAlsa(A)                                                                                                   \
  if(auto res = A)                                                                                                     \
  std::cerr << "Alsa Audio Error: " << #A << " throws error: " << snd_strerror(res) << std::endl

namespace
{

  constexpr auto c_numPeriods = 3;
  constexpr auto c_framesPerPeriod = 512;

  struct S16
  {
    static constexpr snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S16;
    static constexpr float FLOAT_TO_INT = static_cast<float>((1 << 14) - 1);
    using Sample = int16_t;

    inline static Sample fromFloat(float f)
    {
      return static_cast<Sample>(f * FLOAT_TO_INT);
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
      auto q = static_cast<int32_t>(f * FLOAT_TO_INT);
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
      return static_cast<Sample>(f * FLOAT_TO_INT);
    }
  };

  template <typename SampleTrait> struct Frame2Ch
  {
    Frame2Ch() = default;

    explicit Frame2Ch(const Dsp::OutFrame &in)
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

    explicit Frame4Ch(const Dsp::OutFrame &in)
        : mainLeft { SampleTrait::fromFloat(in.main.left) }
        , mainRight { SampleTrait::fromFloat(in.main.right) }
        , preLeft { SampleTrait::fromFloat(in.pre.left) }
        , preRight { SampleTrait::fromFloat(in.pre.right) }
    {
    }

    SampleTrait::Sample mainLeft;
    SampleTrait::Sample mainRight;
    SampleTrait::Sample preLeft;
    SampleTrait::Sample preRight;
  };
}

namespace Audio
{
  AlsaOut::AlsaOut(Dsp::Api::Realtime::Interface &dsp, const std::string &device, int bits, int channels)
  {
    m_audioThread = std::async(std::launch::async,
                               [this, &dsp, device, bits, channels]
                               {
                                 if(bits == 16)
                                   audioThread<S16>(dsp, device, channels);
                                 else if(bits == 24)
                                   audioThread<S24>(dsp, device, channels);
                                 else
                                   audioThread<S32>(dsp, device, channels);
                               });
  }

  AlsaOut::~AlsaOut()
  {
    m_quit = true;
    m_audioThread.wait();
  }

  template <typename SampleTrait>
  void AlsaOut::audioThread(Dsp::Api::Realtime::Interface &dsp, const std::string &device, int channels)
  {
    if(channels == 2)
      audioThread<SampleTrait, Frame2Ch<SampleTrait>, 2>(dsp, device);
    else if(channels == 4)
      audioThread<SampleTrait, Frame4Ch<SampleTrait>, 4>(dsp, device);
    else
      throw std::invalid_argument("num channels not supported");
  }

  template <typename SampleTrait, typename AlsaFrame, int numChannels>
  void AlsaOut::audioThread(Dsp::Api::Realtime::Interface &dsp, const std::string &device)
  {
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

    sched_param p { sched_get_priority_max(SCHED_FIFO) };
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &p);

    AlsaFrame buffer[c_framesPerPeriod] {};

    for(int i = 0; i < c_numPeriods; i++)
      snd_pcm_writei(pcm, buffer, c_framesPerPeriod);

    snd_pcm_start(pcm);

    Dsp::OutFrame samples[c_framesPerPeriod] = {};

    while(!m_quit)
    {
      dsp.doAudio(samples, c_framesPerPeriod, [](const Dsp::MidiEvent &) {});
      std::transform(samples, samples + c_framesPerPeriod, buffer,
                     [](const Dsp::OutFrame &in) { return AlsaFrame(in); });
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