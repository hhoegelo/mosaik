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

  struct Busses
  {
    StereoFrame main {};
    StereoFrame delay {};
    StereoFrame reverb {};
    StereoFrame pre {};
  };

  inline StereoFrame operator+(const StereoFrame &lhs, const StereoFrame &rhs)
  {
    return { lhs.left + rhs.left, lhs.right + rhs.right };
  }

  inline StereoFrame &operator+=(StereoFrame &lhs, const StereoFrame &rhs)
  {
    lhs.left += rhs.left;
    lhs.right += rhs.right;
    return lhs;
  }

  inline StereoFrame operator*(const StereoFrame &lhs, float f)
  {
    return { lhs.left * f, lhs.right * f };
  }

  inline OutFrame operator*(const OutFrame &lhs, float f)
  {
    return { lhs.main * f, lhs.pre * f };
  }

  using SampleBuffer = std::vector<StereoFrame>;
  using SharedSampleBuffer = std::shared_ptr<SampleBuffer>;

  constexpr auto c_maxVolStep = 500.0f / SAMPLERATE;
  constexpr int c_silenceDB = -60;
  constexpr int c_maxDB = 12;
}