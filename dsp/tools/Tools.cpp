#include "Tools.h"
#include <gstreamermm.h>
#include <format>
#include <iostream>

namespace Dsp
{
  std::vector<StereoFrame> Tools::loadFile(const std::filesystem::path &path)
  {
    Gst::init();

    auto desc = std::format("filesrc location=\"{}\" ! decodebin ! audioconvert ! audioresample ! "
                            "audio/x-raw,format=F32LE,channels=2,rate={},layout=interleaved ! appsink name=sink",
                            path.string(), SAMPLERATE);

    auto pipeline = Glib::RefPtr<Gst::Pipeline>::cast_dynamic(Gst::Parse::launch(desc));
    auto sink = Glib::RefPtr<Gst::AppSink>::cast_dynamic(pipeline->get_child("sink"));
    pipeline->use_clock(Glib::RefPtr<Gst::Clock>(nullptr));

    auto res = pipeline->set_state(Gst::State::STATE_PLAYING);
    Gst::State state = Gst::State::STATE_NULL;
    Gst::State pending = Gst::State::STATE_NULL;
    auto r = pipeline->get_state(state, pending, Gst::CLOCK_TIME_NONE);

    std::vector<StereoFrame> result;

    while(!sink->property_eos().get_value())
    {
      if(auto s = sink->pull_sample())
      {
        auto buffer = s->get_buffer();
        Gst::MapInfo map;
        if(buffer->map(map, Gst::MAP_READ))
        {
          auto raw = reinterpret_cast<StereoFrame *>(map.get_data());
          result.insert(result.end(), raw, raw + (map.get_size() / sizeof(StereoFrame)));
        }
      }
    }

    return result;
  }

}