#pragma once

#include <memory>
#include <sigc++/connection.h>
#include <map>
#include "MidiEventDispatcher.h"
#include "ui/Controller.h"

namespace Core::Api
{
  class Interface;
}

namespace Midi
{
  class Alsa;
  class Monitor;
}

namespace Dsp::Api::Display
{
  class Interface;
}

namespace Ui
{
  class Controller;

  namespace Touch
  {
    class Interface;
  }

  namespace Midi
  {
    class Ui : public Interface
    {
     public:
      Ui(std::string midiDevice, Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp,
         ::Ui::Touch::Interface &touchUi, ::Ui::Controller &controller);
      ~Ui() override;

      void setLed(Knob k, Color c) override;
      void setLed(SoftButton s, Color c) override;
      void setLed(Step s, Color c) override;
      void setLed(SoftButton s, uint8_t r, uint8_t g, uint8_t b) override;

     private:
      bool checkForMidiDevices();
      void scheduleLedUpdate();

      std::string m_midiDevice;
      ::Ui::Controller &m_controller;
      std::unique_ptr<::Midi::Monitor> m_monitor;
      sigc::connection m_timer;
      sigc::connection m_ledUpdater;
      std::map<std::string, std::unique_ptr<::Midi::Alsa>> m_inputDevices;
      MidiEventDispatcher m_dispatcher;
    };
  }
}
