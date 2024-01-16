#pragma once

#include <dsp/MidiEvent.h>
#include <dsp/Types.h>

#include <utility>
#include <span>
#include <functional>

namespace Dsp::Api::Realtime
{
  class Interface
  {
   public:
    Interface() = default;
    virtual ~Interface() = default;

    using SendMidi = std::function<void(const MidiEvent &)>;

    virtual void doAudio(const std::span<OutFrame> &out, const SendMidi &cb) = 0;
    virtual void doMidi(const MidiEvent &inEvent) = 0;
  };
}
