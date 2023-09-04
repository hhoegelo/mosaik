#pragma once

#include <stdint.h>
#include <string>
#include <memory>

namespace Dsp
{
  namespace Api
  {
    namespace Control
    {
      using Col = uint8_t;
      using Row = uint8_t;

      class Interface
      {
       public:
        virtual ~Interface() = default;

        virtual void updateSomeChannelParameter(Col col, Row row, float v) = 0;
        virtual void alsoDoSomethingElseForTheWholeColumn(Col col) = 0;
      };
    }
  }
}