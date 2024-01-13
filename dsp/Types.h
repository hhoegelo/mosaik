#pragma once

namespace Dsp
{
  using Sample = float;

  struct StereoFrame
  {
    Sample left;
    Sample right;
  };

  using InFrame = StereoFrame;

  struct OutFrame
  {
    StereoFrame main;
    StereoFrame pre;
  };
}