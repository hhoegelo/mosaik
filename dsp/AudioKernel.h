#pragma once

#include <dsp/Types.h>
#include <stdint.h>
#include <memory>
#include <vector>
#include <array>

namespace Dsp
{
  struct AudioKernel
  {
    float volume = 0.0;
    FramePos framesPer16th = 1;
    FramePos framesPerLoop = 1;

    struct Tile
    {
      SharedSampleBuffer audio { std::make_shared<SampleBuffer>() };
      std::vector<uint64_t> triggers;  // sample positions where to (re-)start the sample
      float gainLeft { 0.0f };
      float gainRight { 0.0f };
      int8_t playbackFrameIncrement { 0 };
    };

    Tile tiles[NUM_TILES];
  };
}