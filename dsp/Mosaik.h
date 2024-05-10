#pragma once

#include <dsp/Types.h>
#include <array>
#include <limits>

#include "Tile.h"
#include "AudioKernel.h"
#include "PointerExchange.h"
#include "Channel.h"
#include "Reverb.h"

namespace Dsp
{
  class Mosaik
  {
   public:
    Mosaik();

    struct ToUi
    {
      FramePos currentLoopPosition = 0;
      std::array<Channel::ToUi, NUM_CHANNELS> channels;
    };

    void set(AudioKernel *pKernel);
    OutFrame doAudio();
    ToUi &getUiInfo();

   private:
    OutFrame doMainPlayground(const OutFrame &in, float p1, float p2, float p3, float p4, float p5, float p6, float p7);

    ToUi m_toUi;
    FramePos m_position = 0;
    FramePos m_knownFramesPerLoop = 0;
    float m_volume = 1.0f;

    Reverb m_reverb;

    uint8_t m_prelistenInteractionCounter = 0;
    int64_t m_prelistenSamplePosition = 0;
    std::chrono::system_clock::time_point m_sequencerStartTime;

    std::array<Channel, NUM_CHANNELS> m_channels;
    PointerExchange<AudioKernel> m_audioKernel;
  };

}