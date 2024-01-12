#include "AlsaIn.h"
#include <alsa/asoundlib.h>
#include <iostream>
#include <glibmm/main.h>
#include <giomm/unixinputstream.h>

#define checkAlsa(A)                                                                                                   \
  if(auto res = A)                                                                                                     \
  std::cerr << "Alsa Midi Error: " << #A << " throws error: " << snd_strerror(res) << std::endl

namespace Midi
{

  AlsaIn::AlsaIn(const std::string &device, const Callback &cb)
      : m_cb(cb)
  {
    checkAlsa(snd_rawmidi_open(&m_device, nullptr, device.c_str(), SND_RAWMIDI_NONBLOCK));

    pollfd polls[1];
    snd_rawmidi_poll_descriptors(m_device, polls, 1);

    auto channel = g_io_channel_unix_new(polls[0].fd);

    m_id = g_io_add_watch(
        channel, (GIOCondition) (G_IO_IN | G_IO_HUP | G_IO_ERR),
        +[](GIOChannel *source, GIOCondition condition, gpointer data) -> gboolean
        {
          if(condition & G_IO_IN)
          {
            auto pThis = static_cast<AlsaIn *>(data);
            MidiEvent event;

            if(snd_rawmidi_read(pThis->m_device, event.begin(), event.size()) == 3)
              pThis->m_cb(event);
          }

          return true;
        },
        this);
    g_io_channel_unref(channel);
  }

  AlsaIn::~AlsaIn()
  {
    g_source_remove(m_id);
    snd_rawmidi_close(m_device);
  }

}