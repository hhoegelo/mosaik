#include <memory>
#include "Ui.h"
#include <midi/Monitor.h>
#include <midi/AlsaIn.h>
#include <glibmm.h>

namespace Ui::Midi
{
  Ui::Ui(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp)
      : m_monitor(std::make_unique<::Midi::Monitor>())
  {
    Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &Ui::checkForMidiDevices), 1);
  }

  bool Ui::checkForMidiDevices()
  {
    m_monitor->poll([this](auto &foundDevice)
                    { m_devices[foundDevice] = std::make_unique<::Midi::AlsaIn>(foundDevice); },
                    [this](auto lostDevice) { m_devices.erase(lostDevice); });
    return true;
  }

  Ui::~Ui() = default;
}
