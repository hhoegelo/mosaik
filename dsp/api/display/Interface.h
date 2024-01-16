#pragma once

#include <chrono>

namespace Dsp::Api::Display
{
  class Interface
  {
   public:
    Interface() = default;
    virtual ~Interface() = default;

    using Position = std::chrono::nanoseconds;

    virtual Position getCurrentPosition() const = 0;
  };
}
