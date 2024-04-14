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

    auto currentLoopPosition = (m_position++ % kernel->framesPerLoop);
    m_toUi.currentLoopPosition = currentLoopPosition;

    OutFrame frame {};

    for(auto c = 0; c < NUM_TILES; c++)
      frame.main = frame.main + m_tiles[c].doAudio(kernel->tiles[c], m_toUi.tiles[c], currentLoopPosition);

    m_volume += std::clamp(::Tools::dBToFactor<c_silenceDB, c_maxDB>(kernel->volume_dB) - m_volume, -c_maxVolStep,
                           c_maxVolStep);

    if(std::exchange(m_prelistenInteractionCounter, kernel->prelistenInteractionCounter)
       != kernel->prelistenInteractionCounter)
      m_prelistenSamplePosition = 0;

    if(m_prelistenSamplePosition < kernel->prelistenSample->size())
      frame.pre = frame.pre + kernel->prelistenSample->at(m_prelistenSamplePosition++);

    return frame * m_volume;
  }

  void Mosaik::set(AudioKernel *pKernel)
  {
    m_audioKernel.set(pKernel);
  }

  Mosaik::ToUi &Mosaik::getUiInfo()
  {
    return m_toUi;
  }

}