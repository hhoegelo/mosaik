#pragma once

#include <string>
namespace Midi
{
  class AlsaIn
  {
   public:
    AlsaIn(const std::string &device);
  };
}
