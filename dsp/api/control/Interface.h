#pragma once

#include <stdint.h>
#include <string>
#include <memory>
#include <filesystem>

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

        virtual void loadSample(Col col, Row row, const std::filesystem::path& path) = 0;
      };
    }
  }
}