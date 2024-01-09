#pragma once

#include <functional>
#include <set>
#include <string>
#include <alsa/asoundlib.h>

namespace Midi
{
  class Monitor
  {
   public:
    Monitor();

    void poll(std::function<void(const std::string &)> foundDevices,
              std::function<void(const std::string &)> lostDevices);

   private:
    using DeviceNames = std::set<std::string>;

    void recurseCards(DeviceNames &ins, int card) const;
    void recurseCard(DeviceNames &ins, int card) const;

    void recurseDevices(DeviceNames &ins, snd_ctl_t *ctl, int card, int device) const;
    void collectPorts(DeviceNames &target, snd_ctl_t *ctl, _snd_rawmidi_info *info, int card, int device,
                      snd_rawmidi_stream_t stream) const;

    DeviceNames m_knownInputs;
  };

}  // Midi
