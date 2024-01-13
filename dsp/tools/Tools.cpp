#include "Tools.h"
#include <gst/gst.h>

namespace Dsp
{
  std::vector<StereoFrame> Tools::loadFile(const std::filesystem::path &path, int sampleRate)
  {
    gst_init(nullptr, nullptr);
    return std::vector<StereoFrame>();
  }

}