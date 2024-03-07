#include "AlsaOut.h"
#include <dsp/MidiEvent.h>
#include <dsp/api/realtime/Interface.h>
#include <alsa/asoundlib.h>

#include <iostream>

#define checkAlsa(A)                                                                                                   \
  if(auto res = A)                                                                                                     \
  std::cerr << "Alsa Audio Error: " << #A << " throws error: " << snd_strerror(res) << std::endl

constexpr auto c_channels = 2;
constexpr auto c_numPeriods = 8;
constexpr auto c_framesPerPeriod = 256;

struct S16
{
  static constexpr snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S16;
  static constexpr float FLOAT_TO_INT = static_cast<float>((1 << 14) - 1);
  using Sample = int16_t;
};

struct S32
{
  static constexpr snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S32;
  static constexpr float FLOAT_TO_INT = static_cast<float>((1 << 30) - 1);
  using Sample = int32_t;
};

namespace Audio
{

  AlsaOut::AlsaOut(Dsp::Api::Realtime::Interface &dsp, const std::string &device, int bits)
  {
    m_audioThread = std::async(std::launch::async,
                               [this, &dsp, device, bits]
                               {
                                 if(bits == 16)
                                   audioThread<S16>(dsp, device);
                                 else
                                   audioThread<S32>(dsp, device);
                               });
  }

  AlsaOut::~AlsaOut()
  {
    m_quit = true;
    m_audioThread.wait();
  }

  template <typename SampleTrait>
  void AlsaOut::audioThread(Dsp::Api::Realtime::Interface &dsp, const std::string &device)
  {
    struct StereoFrame
    {
      SampleTrait::Sample left;
      SampleTrait::Sample right;
    };

    snd_pcm_t *pcm = nullptr;
    checkAlsa(snd_pcm_open(&pcm, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0));

    snd_pcm_hw_params_t *hwparams = nullptr;
    snd_pcm_hw_params_alloca(&hwparams);

    snd_pcm_hw_params_any(pcm, hwparams);
    checkAlsa(snd_pcm_hw_params_set_access(pcm, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED));
    checkAlsa(snd_pcm_hw_params_set_format(pcm, hwparams, SampleTrait::FORMAT));

    checkAlsa(snd_pcm_hw_params_set_channels(pcm, hwparams, c_channels));
    checkAlsa(snd_pcm_hw_params_set_rate(pcm, hwparams, SAMPLERATE, 0));

    checkAlsa(snd_pcm_hw_params_set_periods(pcm, hwparams, c_numPeriods, 0));
    checkAlsa(snd_pcm_hw_params_set_period_size(pcm, hwparams, c_framesPerPeriod, 0));

    checkAlsa(snd_pcm_hw_params(pcm, hwparams));

    checkAlsa(snd_pcm_prepare(pcm));

    sched_param p { sched_get_priority_max(SCHED_FIFO) };
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &p);

    StereoFrame buffer[c_framesPerPeriod] = {};

    for(int i = 0; i < c_numPeriods; i++)
      snd_pcm_writei(pcm, buffer, c_framesPerPeriod);

    snd_pcm_start(pcm);

    Dsp::OutFrame samples[c_framesPerPeriod] = {};

    while(!m_quit)
    {
      dsp.doAudio(samples, c_framesPerPeriod, [](const Dsp::MidiEvent &) {});

      std::transform(
          samples, samples + c_framesPerPeriod, buffer,
          [](const Dsp::OutFrame &in)
          {
            return StereoFrame { static_cast<SampleTrait::Sample>(in.main.left * SampleTrait::FLOAT_TO_INT),
                                 static_cast<SampleTrait::Sample>(in.main.right * SampleTrait::FLOAT_TO_INT) };
          });

      snd_pcm_writei(pcm, buffer, c_framesPerPeriod);
    }

    snd_pcm_close(pcm);
  }

}