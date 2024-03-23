#pragma once

#include <memory>
#include <sigc++/connection.h>
#include <map>
#include "MidiEventDispatcher.h"
#include "Controller.h"

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
  class SharedState;

  namespace Midi
  {
    class Ui : public Interface
    {
     public:
      Ui(SharedState &sharedUiState, std::string midiDevice, Core::Api::Interface &core,
         Dsp::Api::Display::Interface &dsp);
      ~Ui() override;

      void setSoftButtonColor(SoftButton button, Color c) override;
      void setStepButtonColor(Step step, Color c) override;
      void highlightCurrentStep(Step oldStep, Step newStep) override;

     private:
      bool checkForMidiDevices();

      std::string m_midiDevice;
      std::unique_ptr<::Midi::Monitor> m_monitor;
      sigc::connection m_timer;
      std::map<std::string, std::unique_ptr<::Midi::Alsa>> m_inputDevices;
      Controller m_controller;
      MidiEventDispatcher m_dispatcher;
    };
  }
}
