#pragma once

#include <core/Types.h>
#include "tools/ReactiveVar.h"

namespace Core
{

  struct FadeParameterSanitizer
  {
    static void sanitizeFadeInPos(size_t sampleLength, Tools::ReactiveVar<FramePos> &fadeInPos,
                                  Tools::ReactiveVar<FramePos> &fadeInLen, Tools::ReactiveVar<FramePos> &fadeOutPos,
                                  Tools::ReactiveVar<FramePos> &fadeOutLen);
    static void sanitizeFadeInLen(size_t sampleLength, const FramePos &fadeInPos,
                                  Tools::ReactiveVar<FramePos> &fadeInLen, Tools::ReactiveVar<FramePos> &fadeOutPos,
                                  Tools::ReactiveVar<FramePos> &fadeOutLen);
    static void sanitizeFadeOutPos(size_t sampleLength, const FramePos &fadeInPos, const FramePos &fadeInLen,
                                   Tools::ReactiveVar<FramePos> &fadeOutPos, Tools::ReactiveVar<FramePos> &fadeOutLen);
    static void sanitizeFadeOutLen(size_t sampleLength, const FramePos &fadeInPos, const FramePos &fadeInLen,
                                   const FramePos &fadeOutPos, Tools::ReactiveVar<FramePos> &fadeOutLen);
  };
}