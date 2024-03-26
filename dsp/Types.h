#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

namespace Dsp
{
  using Sample = float;
  using Step = uint8_t;

  using FramePos = int64_t;

  struct StereoFrame
  {
    Sample left;
    Sample right;
  };

  struct OutFrame
  {
    StereoFrame main;
    StereoFrame pre;
  };

  inline StereoFrame operator+(const StereoFrame &lhs, const StereoFrame &rhs)
  {
    return { lhs.left + rhs.left, lhs.right + rhs.right };
  }

  inline StereoFrame operator*(const StereoFrame &lhs, float f)
  {
    return { lhs.left * f, lhs.right * f };
  }

  using SampleBuffer = std::vector<StereoFrame>;
  using SharedSampleBuffer = std::shared_ptr<SampleBuffer>;
}