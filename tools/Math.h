#pragma once

#include <array>
#include <valarray>
#include <cassert>

namespace Tools
{

  namespace detail
  {
    template <int min_dB, int max_dB> std::array<float, max_dB - min_dB + 1> initTable()
    {
      std::array<float, max_dB - min_dB + 1> arr;
      for(int i = 0; i < arr.size(); i++)
      {
        arr[i] = std::pow(10.0f, (i + min_dB) / 20.f);
      }
      return arr;
    }
  }

  template <int min_dB, int max_dB> static float dBToFactor(float db)
  {
    static auto table = detail::initTable<min_dB, max_dB>();
    auto i = std::clamp(static_cast<int>(std::round(db)), min_dB, max_dB) - min_dB;
    assert(i >= 0);
    assert(i < table.size());

    return table[i];
  }
}