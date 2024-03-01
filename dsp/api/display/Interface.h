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

    [[nodiscard]] virtual Step getCurrentStep() const = 0;
    [[nodiscard]] virtual float getCurrentTileLevel(Core::TileId tileId) = 0;
  };
}
