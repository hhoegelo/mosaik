#include "Channel.h"

Dsp::StereoFrame Dsp::Channel::doAudio(Dsp::AudioKernel::Channel channel, Dsp::Channel::ToUi &ui, Dsp::FramePos i)
{
  Dsp::StereoFrame r {};

  for(auto t = 0; t < NUM_TILES_PER_CHANNEL; t++)
    r = r + m_tiles[t].doAudio(channel.tiles[t], ui.tiles[t], i);

  return r;
}
