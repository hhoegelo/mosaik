#pragma once

#include <chrono>

namespace Dsp
{
  namespace Api
  {
    namespace Display
    {
      class Api
      {
       public:
        Api()
        {
        }

        using Position = std::chrono::nanoseconds;

        virtual Position getCurrentPosition() const = 0;
      };
    }
  }
}