#include "dsp/Dsp.h"
#include "core/Core.h"
#include "audio/AlsaOut.h"
#include "ui/midi-ui/Ui.h"
#include "ui/touch-ui/Ui.h"
#include "midi/Monitor.h"
#include "ui/midi-ui/DebugUI.h"
#include "core/DataModel.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <glibmm.h>

int main(int args, char** argv)
{
  using namespace boost::program_options;
  using namespace std;

  variables_map vm;

  try
  {
    options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")("alsa-out", value<std::string>(), "output device")(
        "bits", value<int>(), "16 or 32 bit")("midi-ui", value<std::string>(),
                                              "alsa midi device for the mosaik hardware UI");

    store(parse_command_line(args, argv, desc), vm);
    notify(vm);

    if(vm.count("help"))
    {
      cout << desc << "\n";
      return 1;
    }
  }
  catch(const std::exception& e)
  {
    std::cout << "error parsing options: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  std::string alsaOut = vm.count("alsa-out") ? vm["alsa-out"].as<std::string>() : "pulse";
  std::string midiUi = vm.count("midi-ui") ? vm["midi-ui"].as<std::string>() : "";
  int bits = vm.count("bits") ? vm["bits"].as<int>() : 16;

  Glib::init();

  Dsp::Dsp dsp;
  Core::Core core(dsp.getControlApi());
  Audio::AlsaOut audioOut(dsp.getRealtimeApi(), alsaOut, bits);
  Ui::Touch::Ui touchUI(core.getApi(), dsp.getDisplayApi());
  Ui::Midi::Ui midiUI(midiUi, core.getApi(), dsp.getDisplayApi(), touchUI.getApi());
  Ui::Midi::DebugUI dbg(core.getApi(), dsp.getDisplayApi(), touchUI.getApi());

  touchUI.attach(dbg);
  touchUI.run();

  return EXIT_SUCCESS;
}
