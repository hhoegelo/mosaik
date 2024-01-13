#pragma once

#include <stdint.h>
#include <filesystem>

namespace Core
{
  using Col = uint8_t;
  using Row = uint8_t;
  using ParameterValue = float;
  using Path = std::filesystem::path;

  static constexpr auto NUM_COLS = 16;
  static constexpr auto NUM_ROWS = 16;

  struct DataModel
  {
    struct Channel
    {
      Path sample;
    };

    using Column = std::array<Channel, NUM_ROWS>;
    using Matrix = std::array<Column, NUM_COLS>;
    Matrix matrix;

    ParameterValue bpm;
    ParameterValue mainVolume;
  };

}