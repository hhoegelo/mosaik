
#pragma once

#include <dsp/Tile.h>

namespace Dsp
{
  class Channel
  {
   public:
    struct ToUi
    {
      std::array<Tile::ToUi, NUM_TILES_PER_CHANNEL> tiles;
    };

    StereoFrame doAudio(AudioKernel::Channel channel, ToUi &ui, FramePos i);

   private:
    std::array<Tile, NUM_TILES_PER_CHANNEL> m_tiles;
  };
}