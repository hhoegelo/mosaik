#include <dsp/Mosaik.h>
#include <tools/Math.h>
#include <algorithm>
#include <utility>

namespace Dsp
{

  Mosaik::Mosaik()
      : m_audioKernel(new AudioKernel)
  {
  }

  OutFrame Mosaik::doAudio()
  {
    auto kernel = m_audioKernel.get();

    if(!m_knownFramesPerLoop)
      m_knownFramesPerLoop = kernel->framesPerLoop;

    if(m_knownFramesPerLoop != kernel->framesPerLoop)
    {
      auto oldLoopPositionRel
          = static_cast<double>(m_position % m_knownFramesPerLoop) / static_cast<double>(m_knownFramesPerLoop);

      m_position = static_cast<FramePos>(oldLoopPositionRel * static_cast<double>(kernel->framesPerLoop));
    }

    m_knownFramesPerLoop = kernel->framesPerLoop;

    if(std::exchange(m_sequencerStartTime, kernel->sequencerStartTime) != kernel->sequencerStartTime)
    {
      size_t diffMS = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()
                                                                            - m_sequencerStartTime)
                          .count();
      size_t framesSinceStart = diffMS * SAMPLERATE / 1000;
      double posInLoop = fmod(static_cast<double>(framesSinceStart), static_cast<double>(m_knownFramesPerLoop));
      m_position = std::round(posInLoop);
    }

    auto currentLoopPosition = (m_position++ % kernel->framesPerLoop);
    m_toUi.currentLoopPosition = currentLoopPosition;

    Busses busses;

    for(auto c = 0; c < NUM_CHANNELS; c++)
      m_channels[c].doAudio(busses, kernel->channels[c], m_toUi.channels[c], currentLoopPosition);

    m_volume += std::clamp(::Tools::dBToFactor<c_silenceDB, c_maxDB>(kernel->volume_dB) - m_volume, -c_maxVolStep,
                           c_maxVolStep);

    if(std::exchange(m_prelistenInteractionCounter, kernel->prelistenInteractionCounter)
       != kernel->prelistenInteractionCounter)
      m_prelistenSamplePosition = 0;

    if(m_prelistenSamplePosition < kernel->prelistenSample->size())
      busses.pre = busses.pre + kernel->prelistenSample->at(m_prelistenSamplePosition++);

    auto reverbSize = kernel->reverbRoomSize * (2 - kernel->reverbRoomSize);

    m_reverb.set(reverbSize, kernel->reverbChorus, 0, (200.f * SAMPLERATE / 1000.f) * kernel->reverbPreDelay,
                 kernel->reverbColor);

    m_reverb.apply(busses.reverb.left, busses.reverb.right, 1.0, 0.5f, 0.0f, 1.0f);

    auto revL = m_reverb.m_out_L;
    auto revR = m_reverb.m_out_R;

    m_reverbReturnFactor
        += std::clamp(::Tools::dBToFactor<c_silenceDB, c_maxDB>(kernel->reverbReturn) - m_reverbReturnFactor,
                      -c_maxVolStep, c_maxVolStep);

    busses.main.left += revL * m_reverbReturnFactor * kernel->reverbOnOff;
    busses.main.right += revR * m_reverbReturnFactor * kernel->reverbOnOff;
    return { busses.main * m_volume, busses.pre };
    /*
    return doMainPlayground(frame, kernel->mainPlayground1, kernel->mainPlayground2, kernel->mainPlayground3,
                            kernel->mainPlayground4, kernel->mainPlayground5, kernel->mainPlayground6,
                            kernel->mainPlayground7)
        * m_volume;*/
  }

  void Mosaik::set(AudioKernel *pKernel)
  {
    m_audioKernel.set(pKernel);
  }

  Mosaik::ToUi &Mosaik::getUiInfo()
  {
    return m_toUi;
  }

  OutFrame Mosaik::doMainPlayground(const OutFrame &in, float p1, float p2, float p3, float p4, float p5, float p6,
                                    float p7)
  {
    float ml = in.main.left;
    float mr = in.main.right;
    float pl = in.pre.left;
    float pr = in.pre.right;

    float out_l = (p1 * ml) + (1 - p1) * pl;
    float out_r = (p1 * mr) + (1 - p1) * pr;

    OutFrame output { out_l, out_r, in.main.left, in.main.right };

    return output;
    //return in;
  }

}
