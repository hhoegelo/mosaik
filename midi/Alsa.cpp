#include "Alsa.h"
#include <alsa/asoundlib.h>
#include <iostream>
#include <utility>
#include <glibmm/main.h>
#include <giomm/unixinputstream.h>

#define checkAlsa(A)                                                                                                   \
  if(auto res = A)                                                                                                     \
  std::cerr << "Alsa Midi Error: " << #A << " throws error: " << snd_strerror(res) << std::endl

namespace Midi
{

  Alsa::Alsa(const std::string &device, Callback cb)
      : m_cb(std::move(cb))
  {
    checkAlsa(snd_rawmidi_open(&m_input, &m_output, device.c_str(), SND_RAWMIDI_NONBLOCK));

    pollfd polls[1];
    snd_rawmidi_poll_descriptors(m_input, polls, 1);

    auto channel = g_io_channel_unix_new(polls[0].fd);

    m_id = g_io_add_watch(
        channel, (GIOCondition) (G_IO_IN | G_IO_HUP | G_IO_ERR),
        +[](GIOChannel *source, GIOCondition condition, gpointer data) -> gboolean
        {
          if(condition & G_IO_IN)
          {
            auto pThis = static_cast<Alsa *>(data);
            MidiEvent event;

            if(snd_rawmidi_read(pThis->m_input, event.begin(), event.size()) == 3)
              pThis->m_cb(event);
          }

          return true;
        },
        this);
    g_io_channel_unref(channel);
  }

  Alsa::~Alsa()
  {
    g_source_remove(m_id);
    snd_rawmidi_close(m_input);
    snd_rawmidi_close(m_output);
  }

  void Alsa::send(const Alsa::MidiEvent &event)
  {
    if(m_output)
    {
      snd_rawmidi_write(m_output, event.data(), event.size());
    }
  }

}