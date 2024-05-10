#include "Channel.h"
#include "tools/Math.h"

void Dsp::Channel::doAudio(Busses &busses, const Dsp::AudioKernel::Channel &kernel, Dsp::Channel::ToUi &ui,
                           Dsp::FramePos i)
{
  Dsp::StereoFrame pre {};

  for(auto t = 0; t < NUM_TILES_PER_CHANNEL; t++)
    pre = pre + m_tiles[t].doAudio(kernel.tiles[t], ui.tiles[t], i);

  m_volume += std::clamp(::Tools::dBToFactor<c_silenceDB, c_maxDB>(kernel.volume_dB) - m_volume, -c_maxVolStep,
                         c_maxVolStep);

  auto post = pre * m_volume;

  m_preReverbFactor += std::clamp(kernel.preReverbFactor - m_preReverbFactor, -c_maxVolStep, c_maxVolStep);
  m_preDelayFactor += std::clamp(kernel.preDelayFactor - m_preDelayFactor, -c_maxVolStep, c_maxVolStep);
  m_postReverbFactor += std::clamp(kernel.postReverbFactor - m_postReverbFactor, -c_maxVolStep, c_maxVolStep);
  m_postDelayFactor += std::clamp(kernel.postDelayFactor - m_postDelayFactor, -c_maxVolStep, c_maxVolStep);
  m_muteFactor += std::clamp(kernel.muteFactor - m_muteFactor, -c_maxVolStep, c_maxVolStep);

  busses.reverb += pre * m_preReverbFactor + post * m_postReverbFactor;
  busses.delay += pre * m_preDelayFactor + post * m_postDelayFactor;
  busses.main += post * m_muteFactor;
}
