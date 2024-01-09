
#include <format>
#include <algorithm>
#include "Monitor.h"
#include "AlsaIn.h"

namespace Midi
{
  Monitor::Monitor() = default;

  void Monitor::collectPorts(DeviceNames &target, snd_ctl_t *ctl, snd_rawmidi_info_t *info, int card, int device,
                             snd_rawmidi_stream_t stream) const
  {
    snd_rawmidi_info_set_stream(info, stream);
    snd_ctl_rawmidi_info(ctl, info);

    for(auto sub = 0; sub < snd_rawmidi_info_get_subdevices_count(info); ++sub)
    {
      snd_rawmidi_info_set_stream(info, stream);
      snd_rawmidi_info_set_subdevice(info, sub);
      snd_ctl_rawmidi_info(ctl, info);
      auto sub_name = snd_rawmidi_info_get_subdevice_name(info);

      if(sub == 0 && sub_name[0] == '\0')
      {
        target.insert(std::format("hw:{},{}", card, device));
        break;
      }
      else
      {
        target.insert(std::format("hw:{},{},{}", card, device, sub));
      }
    }
  }

  void Monitor::recurseDevices(DeviceNames &ins, snd_ctl_t *ctl, int card, int device) const
  {
    if(snd_ctl_rawmidi_next_device(ctl, &device) == 0)
    {
      if(device >= 0)
      {
        snd_rawmidi_info_t *info;
        snd_rawmidi_info_alloca(&info);
        snd_rawmidi_info_set_device(info, device);
        collectPorts(ins, ctl, info, card, device, SND_RAWMIDI_STREAM_INPUT);
        recurseDevices(ins, ctl, card, device);
      }
    }
  }

  void Monitor::recurseCard(DeviceNames &ins, int card) const
  {
    snd_ctl_t *ctl;
    char name[32];

    sprintf(name, "hw:%d", card);

    if(snd_ctl_open(&ctl, name, 0) == 0)
    {
      recurseDevices(ins, ctl, card, -1);
      snd_ctl_close(ctl);
    }
  }

  void Monitor::recurseCards(DeviceNames &ins, int card) const
  {
    if(snd_card_next(&card) == 0)
    {
      if(card >= 0)
      {
        recurseCard(ins, card);
        recurseCards(ins, card);
      }
    }
  }
  void Monitor::poll(std::function<void(const std::string &)> foundDevices,
                     std::function<void(const std::string &)> lostDevices)
  {
    DeviceNames ins;

    recurseCards(ins, -1);

    if(m_knownInputs != ins)
    {
      std::vector<std::string> missing;
      std::set_difference(m_knownInputs.begin(), m_knownInputs.end(), ins.begin(), ins.end(),
                          std::back_inserter(missing));

      std::for_each(missing.begin(), missing.end(), [&lostDevices](auto m) { lostDevices(m); });

      std::vector<std::string> newFound;
      std::set_difference(ins.begin(), ins.end(), m_knownInputs.begin(), m_knownInputs.end(),
                          std::back_inserter(newFound));

      std::for_each(newFound.begin(), newFound.end(), [&foundDevices](auto m) { foundDevices(m); });

      m_knownInputs = ins;
    }
  }
}  // Midi