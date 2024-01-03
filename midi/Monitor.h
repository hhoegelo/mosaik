#pragma once

#include <memory>
#include <functional>
#include <set>
#include <map>
#include <string>
#include <alsa/asoundlib.h>

namespace Midi
{
  class AlsaIn;
  class Monitor
  {
   public:
    Monitor();

    void poll(std::function<void(std::weak_ptr<AlsaIn>)> newDevicesCb);

   private:
    using DeviceNames = std::set<std::string>;
    using Devices = std::map<std::string, std::shared_ptr<AlsaIn>>;

    void recurseCards(DeviceNames &ins, int card) const;
    void recurseCard(DeviceNames &ins, int card) const;

    void recurseDevices(DeviceNames &ins, snd_ctl_t *ctl, int card, int device) const;
    void collectPorts(DeviceNames &target, snd_ctl_t *ctl, _snd_rawmidi_info *info, int card, int device,
                      snd_rawmidi_stream_t stream) const;

    DeviceNames m_knownInputs;
    Devices m_devices;
  };

}  // Midi
