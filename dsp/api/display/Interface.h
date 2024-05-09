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

    [[nodiscard]] virtual FramePos getCurrentLoopPosition() const = 0;
    [[nodiscard]] virtual std::tuple<float, float> getLevel(Core::Address address) = 0;
    [[nodiscard]] virtual FramePos getPosition(Core::Address address) = 0;
    [[nodiscard]] virtual std::chrono::milliseconds getDuration(const std::filesystem::path &file) const = 0;
  };
}
