#include <algorithm>
#include <cmath>
#include "Tile.h"
#include "tools/Math.h"

namespace Dsp
{
  StereoFrame Tile::doAudio(AudioKernel::Tile &kernel, ToUi &ui, FramePos currentLoopPosition)
  {
    const auto &audio = *kernel.audio;

    if(std::binary_search(kernel.triggers.begin(), kernel.triggers.end(), currentLoopPosition))
      m_framePosition = 0;

    StereoFrame result = {};

    auto iFramePos = c_invalidFramePosF32 == m_framePosition ? c_invalidFramePosU64
                                                             : static_cast<FramePos>(std::round(m_framePosition));

    if(iFramePos < audio.size())
    {
      if(kernel.reverse)
        result = audio[audio.size() - 1 - iFramePos];
      else
        result = audio[iFramePos];
    }

    result = doPlayground(result, kernel.playground1, kernel.playground2, kernel.playground3, kernel.playground4,
                          kernel.playground5, kernel.playground6, kernel.playground7);

    auto target_dB = kernel.mute ? c_silenceDB : kernel.gain_dB + doEnvelope(kernel, iFramePos);
    auto targetGain = ::Tools::dBToFactor<c_silenceDB, c_maxDB>(target_dB);
    auto targetGainLeft = (kernel.balance < 0) ? targetGain : targetGain * (1.0f - kernel.balance);
    auto targetGainRight = (kernel.balance > 0) ? targetGain : targetGain * (1.0f + kernel.balance);

    m_gainLeft += std::clamp(targetGainLeft - m_gainLeft, -c_maxVolStep, c_maxVolStep);
    m_gainRight += std::clamp(targetGainRight - m_gainRight, -c_maxVolStep, c_maxVolStep);

    result.left *= m_gainLeft;
    result.right *= m_gainRight;

    ui.currentLevel = std::max({ ui.currentLevel, std::abs(result.left), std::abs(result.right) });
    m_framePosition += m_framePosition != c_invalidFramePosF32 ? kernel.playbackFrameIncrement : 0;
    return result;
  }

  float Tile::doEnvelope(AudioKernel::Tile &kernel, FramePos iFramePos) const
  {
    if(iFramePos != c_invalidFramePosU64)
      for(auto &section : kernel.envelope)
        if(iFramePos >= section.pos)
          return section.b + m_framePosition * section.m;
    return c_silenceDB;
  }

  StereoFrame Tile::doPlayground(const StereoFrame &input, float playgroundParam1, float playgroundParam2,
                                 float playgroundParam3, float playgroundParam4, float playgroundParam5,
                                 float playgroundParam6, float playgroundParam7)
  {
    /*
     * @Daniel:
     *  playgroundParam1 ... 7 are the parameters as set in the UI, for example Cutoff and Resonance.
     *
     *  If you have to have state, for example filter coefficients, you can store them in 'this',
     *  for example m_smoothedPlaygroundSomething = calcFilterCoefficientA(playgroundParam1, playgroundParam2);
     *
     *  If this is an expensive operation, you'll have to check that playgroundParam1 or playgroundParam2
     *  actually changed by comparing the incoming parameters with copies stored in 'this':
     *
     *  bool coefficientsOutdated = false;
     *  coefficientsOutdated |= std::exchange(m_knownPlaygroundParameter1, playgroundParam1) != playgroundParam1;
     *  coefficientsOutdated |= std::exchange(m_knownPlaygroundParameter2, playgroundParam2) != playgroundParam2;
     *
     *  if(coefficientsOutdated)
     *    m_smoothedPlaygroundSomething = calcFilterCoefficientA(playgroundParam1, playgroundParam2);
     *
     */

    StereoFrame output { input.left, input.right };

    return output;
  }
}
