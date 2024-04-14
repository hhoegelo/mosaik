#pragma once

#include <dsp/Types.h>
#include <vector>
#include <filesystem>
#include <functional>
#include <optional>
#include <glibmm/refptr.h>

namespace Gst
{
  class Pipeline;
}

namespace Dsp::Tools
{
  class AudioFileDecoder
  {
   public:
    using Callback = std::function<void(std::optional<std::vector<StereoFrame>> &&)>;
    AudioFileDecoder(const std::filesystem::path &path, const Callback &cb);
    ~AudioFileDecoder();

    bool isDone() const;

   private:
    Callback m_cb;
    Glib::RefPtr<Gst::Pipeline> m_pipeline;
    std::vector<StereoFrame> m_result;
    bool m_isDone = false;
  };

}
