
#pragma once

#include <dsp/Tile.h>

namespace Dsp
{
  class Channel
  {
   public:
    struct ToUi
    {
      float levelLeft { 0.0f };
      float levelRight { 0.0f };
      std::array<Tile::ToUi, NUM_TILES_PER_CHANNEL> tiles;
    };

    void doAudio(Busses &busses, const AudioKernel::Channel &channel, ToUi &ui, FramePos i);

   private:
    float m_volume { 1.0f };
    float m_preReverbFactor = 1.0f;
    float m_postReverbFactor = 0.0f;
    float m_preDelayFactor = 1.0f;
    float m_postDelayFactor = 0.0f;
    float m_muteFactor = 1.0f;

    std::array<Tile, NUM_TILES_PER_CHANNEL> m_tiles;
  };
}