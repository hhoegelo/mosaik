#pragma once

#include <core/Types.h>

namespace Core
{

  struct FadeParameterSanitizer
  {
    static void sanitizeFadeInPos(size_t sampleLength, FramePos &fadeInPos, FramePos &fadeInLen, FramePos &fadeOutPos,
                                  FramePos &fadeOutLen);
    static void sanitizeFadeInLen(size_t sampleLength, const FramePos &fadeInPos, FramePos &fadeInLen,
                                  FramePos &fadeOutPos, FramePos &fadeOutLen);
    static void sanitizeFadeOutPos(size_t sampleLength, const FramePos &fadeInPos, const FramePos &fadeInLen,
                                   FramePos &fadeOutPos, FramePos &fadeOutLen);
    static void sanitizeFadeOutLen(size_t sampleLength, const FramePos &fadeInPos, const FramePos &fadeInLen,
                                   const FramePos &fadeOutPos, FramePos &fadeOutLen);
  };
}