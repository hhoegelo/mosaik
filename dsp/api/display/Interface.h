#pragma once

#include <chrono>

namespace Dsp
{
  namespace Api
  {
    namespace Display
    {
      class Interface
      {
       public:
        Interface() = default;

        using Position = std::chrono::nanoseconds;

        virtual Position getCurrentPosition() const = 0;
      };
    }
  }
}