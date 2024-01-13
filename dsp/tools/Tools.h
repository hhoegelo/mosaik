#pragma once

#include <dsp/Types.h>
#include <vector>
#include <filesystem>

namespace Dsp
{
  class Tools
  {
   public:
    static std::vector<StereoFrame> loadFile(const std::filesystem::path &path, int sampleRate);
  };
}