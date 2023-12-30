#include "AlsaOut.h"
#include <api/realtime/Interface.h>
#include <alsa/asoundlib.h>
#include <iostream>

#define checkAlsa(A)                                                                                                   \
  if(auto res = A)                                                                                                     \
  std::cerr << "Alsa Error: " << #A << " throws error: " << snd_strerror(res) << std::endl

constexpr auto c_samplerate = 48000;
constexpr auto c_channels = 2;
constexpr auto c_numPeriods = 3;
constexpr auto c_framesPerPeriod = 192;
constexpr auto c_sampleFormat = SND_PCM_FORMAT_S32_LE;
constexpr auto c_floatToS32 = static_cast<float>((1 << 30) - 1);

using Sample = int32_t;
using SampleFrame = std::tuple<Sample, Sample>;

namespace AudioMidi
{
  AlsaOut::AlsaOut(Dsp::Api::Realtime::Interface &dsp, const std::string &device)
  {
    m_audioThread
        = std::async(std::launch::async,
                     [this, &dsp, device]
                     {
                       snd_pcm_t *pcm = nullptr;
                       checkAlsa(snd_pcm_open(&pcm, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0));

                       snd_pcm_hw_params_t *hwparams = nullptr;
                       snd_pcm_hw_params_alloca(&hwparams);

                       checkAlsa(snd_pcm_hw_params_any(pcm, hwparams));
                       checkAlsa(snd_pcm_hw_params_set_access(pcm, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED));
                       checkAlsa(snd_pcm_hw_params_set_format(pcm, hwparams, c_sampleFormat));

                       checkAlsa(snd_pcm_hw_params_set_channels(pcm, hwparams, c_channels));
                       checkAlsa(snd_pcm_hw_params_set_rate(pcm, hwparams, c_samplerate, 1));

                       checkAlsa(snd_pcm_hw_params_set_periods(pcm, hwparams, c_numPeriods, 0));
                       checkAlsa(snd_pcm_hw_params_set_period_size(pcm, hwparams, c_framesPerPeriod, 0));

                       checkAlsa(snd_pcm_hw_params(pcm, hwparams));

                       SampleFrame buffer[c_framesPerPeriod] = {};

                       checkAlsa(snd_pcm_prepare(pcm));

                       for(int i = 0; i < c_numPeriods; i++)
                       {
                         snd_pcm_writei(pcm, buffer, c_framesPerPeriod * c_channels * sizeof(SampleFrame));
                       }

                       sched_param p { sched_get_priority_max(SCHED_FIFO) };
                       pthread_setschedparam(pthread_self(), SCHED_FIFO, &p);

                       Dsp::Api::Realtime::Interface::OutFrame samples[c_framesPerPeriod];

                       while(!m_quit)
                       {
                         std::span s(samples, c_framesPerPeriod);
                         dsp.doAudio(s, [](const Dsp::MidiEvent &) {});

                         std::transform(s.begin(), s.end(), buffer,
                                        [](const Dsp::Api::Realtime::Interface::OutFrame &in)
                                        {
                                          return std::make_tuple(static_cast<Sample>(in.main.first * c_floatToS32),
                                                                 static_cast<Sample>(in.main.first * c_floatToS32));
                                        });

                         snd_pcm_writei(pcm, buffer, c_framesPerPeriod * c_channels * sizeof(SampleFrame));
                       }

                       snd_pcm_close(pcm);
                     });
  }

  AlsaOut::~AlsaOut()
  {
    m_quit = true;
    m_audioThread.wait();
  }
}