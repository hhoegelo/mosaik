#include "AudioFileDecoder.h"
#include <iostream>
#include <atomic>
#include <future>

// WORKAROUND gstreamermm gcc-13 compilation error
#define __atomic_load(A, B, C) (*B) = *A
#include <gstreamermm.h>
#undef __atomic_load

namespace Dsp::Tools
{
  AudioFileDecoder::AudioFileDecoder(const std::filesystem::path &path, const Callback &cb)
      : m_cb(cb)
  {
    Gst::init();

    if(exists(path))
    {
      char txt[1024];
      snprintf(txt, 1024,
               "filesrc location=\"%s\" ! decodebin ! audioconvert ! audioresample ! "
               "audio/x-raw,format=F32LE,channels=2,rate=%d,layout=interleaved ! appsink emit-signals=true name=sink",
               path.string().c_str(), SAMPLERATE);

      m_pipeline = Glib::RefPtr<Gst::Pipeline>::cast_dynamic(Gst::Parse::launch(txt));
      m_pipeline->use_clock(Glib::RefPtr<Gst::Clock>(nullptr));

      auto bus = m_pipeline->get_bus();
      auto sink = Glib::RefPtr<Gst::AppSink>::cast_dynamic(m_pipeline->get_child("sink"));

      bus->add_watch(
          [this, sink, cb = cb](const Glib::RefPtr<Gst::Bus> &, const Glib::RefPtr<Gst::Message> &msg)
          {
            if(msg->get_source().get() == m_pipeline.get())
            {
              if(msg->get_message_type() == Gst::MessageType::MESSAGE_EOS)
              {
                m_isDone = true;
                cb(std::move(m_result));
              }
              if(msg->get_message_type() == Gst::MessageType::MESSAGE_ERROR)
              {
                m_isDone = true;
                cb({});
              }
            }

            return true;
          });

      sink->signal_new_sample().connect(
          [this, sink]
          {
            while(auto s = sink->try_pull_sample(1))
            {
              auto buffer = s->get_buffer();
              Gst::MapInfo map;
              if(buffer->map(map, Gst::MAP_READ))
              {
                auto raw = reinterpret_cast<StereoFrame *>(map.get_data());
                m_result.insert(m_result.end(), raw, raw + (map.get_size() / sizeof(StereoFrame)));
              }
            }
            return Gst::FLOW_OK;
          });

      m_pipeline->set_state(Gst::State::STATE_PLAYING);
    }
    else
    {
      m_isDone = true;
      cb({});
    }
  }

  bool AudioFileDecoder::isDone() const
  {
    return m_isDone;
  }

  AudioFileDecoder::~AudioFileDecoder()
  {
    if(m_pipeline)
      m_pipeline->set_state(Gst::State::STATE_NULL);
  }
}