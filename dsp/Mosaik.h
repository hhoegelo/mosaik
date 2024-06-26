#pragma once

#include <dsp/Types.h>
#include <array>
#include <limits>

#include "Tile.h"
#include "AudioKernel.h"
#include "PointerExchange.h"
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
      std::array<Tile::ToUi, NUM_TILES> tiles;
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
    float m_prelistenVolume = 1.0f;
    float m_reverbReturnFactor = 0;

    Reverb m_reverb;

    uint8_t m_prelistenInteractionCounter = 0;
    int64_t m_prelistenSamplePosition = 0;
    std::chrono::system_clock::time_point m_sequencerStartTime;

    std::array<Tile, NUM_TILES> m_tiles;
    PointerExchange<AudioKernel> m_audioKernel;
  };

}