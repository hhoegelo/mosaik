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

    m_framePosition += m_framePosition != c_invalidFramePosF32 ? kernel.playbackFrameIncrement : 0;

    ui.levelLeft = std::max({ ui.levelLeft, std::abs(result.left) });
    ui.levelRight = std::max({ ui.levelRight, std::abs(result.right) });
    ui.frame = m_framePosition;
    
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
   *    constexpr auto denormal = 1e-13f;
   *
   */
  StereoFrame Tile::doPlayground(const StereoFrame &input, float playgroundParam1, float playgroundParam2,
                                 float playgroundParam3, float playgroundParam4, float playgroundParam5,
                                 float playgroundParam6, float playgroundParam7)
  {
    float m_cutoff = playgroundParam1*100;
    if(m_cutoff < 17)
        m_cutoff = 17;

    float m_reso = playgroundParam2;
    if(m_reso > 0.99)
        m_reso = 0.99;

    float HP { 0.0f };
    float BP { 0.0f };
    float LP { 0.0f };

    // pow(base,expo)
    float F = 8.17742 * pow(1.059, m_cutoff);

    float W = F * (6.28319 / SAMPLERATE);
    if(W > 0.8) W = 0.8;

    float d = 2 * (1 - m_reso);

    HP = input.left - ((m_BPz * d) + m_LPz);
    BP = (HP * W) + m_BPz;
    LP = (BP * W) + m_LPz;

    m_HPz = HP;
    m_BPz = BP;
    m_LPz = LP;

    StereoFrame output { LP, LP };

    return output;
  }
}
