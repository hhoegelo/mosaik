#include <algorithm>
#include "FadeParameterSanitizer.h"

namespace Core
{
  void FadeParameterSanitizer::sanitizeFadeInPos(size_t sampleLength, Tools::ReactiveVar<FramePos> &fadeInPos,
                                                 Tools::ReactiveVar<FramePos> &fadeInLen,
                                                 Tools::ReactiveVar<FramePos> &fadeOutPos,
                                                 Tools::ReactiveVar<FramePos> &fadeOutLen)
  {
    fadeInPos = std::clamp<FramePos>(fadeInPos, 0, sampleLength);
    sanitizeFadeInLen(sampleLength, fadeInPos, fadeInLen, fadeOutPos, fadeOutLen);
  }

  void FadeParameterSanitizer::sanitizeFadeInLen(size_t sampleLength, const FramePos &fadeInPos,
                                                 Tools::ReactiveVar<FramePos> &fadeInLen,
                                                 Tools::ReactiveVar<FramePos> &fadeOutPos,
                                                 Tools::ReactiveVar<FramePos> &fadeOutLen)
  {
    fadeInLen = std::clamp<FramePos>(fadeInLen, 0, static_cast<FramePos>(sampleLength) - fadeInPos);
    sanitizeFadeOutPos(sampleLength, fadeInPos, fadeInLen, fadeOutPos, fadeOutLen);
  }

  void FadeParameterSanitizer::sanitizeFadeOutPos(size_t sampleLength, const FramePos &fadeInPos,
                                                  const FramePos &fadeInLen, Tools::ReactiveVar<FramePos> &fadeOutPos,
                                                  Tools::ReactiveVar<FramePos> &fadeOutLen)
  {
    fadeOutPos = std::clamp<FramePos>(fadeOutPos, fadeInPos + fadeInLen, static_cast<FramePos>(sampleLength));
    sanitizeFadeOutLen(sampleLength, fadeInPos, fadeInLen, fadeOutPos, fadeOutLen);
  }

  void FadeParameterSanitizer::sanitizeFadeOutLen(size_t sampleLength, const FramePos &fadeInPos,
                                                  const FramePos &fadeInLen, const FramePos &fadeOutPos,
                                                  Tools::ReactiveVar<FramePos> &fadeOutLen)
  {
    fadeOutLen = std::clamp<FramePos>(fadeOutLen, 0, static_cast<FramePos>(sampleLength) - fadeOutPos);
  }
}