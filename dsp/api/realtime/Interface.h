#pragma once

#include <utility>
#include <span>
#include <functional>

#include <MidiEvent.h>

namespace Dsp
{
  namespace Api
  {
    namespace Realtime
    {
      class Interface
      {
       public:
        Interface() = default;

        using Sample = float;
        using StereoFrame = std::pair<Sample, Sample>;
        using InFrame = StereoFrame;

        struct OutFrame
        {
          StereoFrame main;
          StereoFrame pre;
        };

        using SendMidi = std::function<void(const MidiEvent &)>;

        virtual void doAudio(const std::span<InFrame> &in, std::span<OutFrame> &out, const SendMidi &cb) = 0;
        virtual void doMidi(const MidiEvent &inEvent) = 0;
      };
    }
  }
}