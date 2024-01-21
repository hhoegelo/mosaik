#pragma once

#include <dsp/Types.h>
#include <core/Types.h>
#include <chrono>

namespace Dsp::Api::Display
{
  class Interface
  {
   public:
    Interface() = default;
    virtual ~Interface() = default;

    virtual Step getCurrentStep() const = 0;
    virtual bool isTileCurrentlyPlaying(Core::TileId tileId) const = 0;
  };
}
