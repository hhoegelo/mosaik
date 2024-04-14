#pragma once

#include "Types.h"
#include "AudioKernel.h"
#include <limits>

namespace Dsp
{
  class Tile
  {
   public:
    struct ToUi
    {
      float currentLevel { 0.0f };
    };

    StereoFrame doAudio(AudioKernel::Tile &kernel, ToUi &ui, FramePos currentLoopPosition);

   private:
    float doEnvelope(AudioKernel::Tile &kernel, FramePos iFramePos) const;

    static constexpr auto c_invalidFramePosF32 = std::numeric_limits<float>::max();
    static constexpr auto c_invalidFramePosU64 = std::numeric_limits<FramePos>::max();

    float m_gainLeft { 1.0f };
    float m_gainRight { 1.0f };
    float m_framePosition = c_invalidFramePosF32;
  };
}
