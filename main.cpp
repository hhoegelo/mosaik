#include "dsp/Dsp.h"
#include "core/Core.h"
#include "audio/AlsaOut.h"
#include "ui/midi-ui/Ui.h"
#include "ui/touch-ui/Ui.h"
#include "midi/Monitor.h"
#include "ui/midi-ui/DebugUI.h"
#include "ui/SharedState.h"
#include "core/DataModel.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <glibmm.h>

int main(int args, char** argv)
{
  try
  {
    using namespace boost::program_options;
    using namespace std;

    options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")("alsa-out", value<std::string>(),
                                                       "output device")("bits", value<int>(), "16 or 32 bit");

    variables_map vm;
    store(parse_command_line(args, argv, desc), vm);
    notify(vm);

    if(vm.count("help"))
    {
      cout << desc << "\n";
      return 1;
    }

    if(!vm.count("alsa-out"))
    {
      cout << "alsa-out is missing.\n";
      return EXIT_SUCCESS;
    }

    int bits = 16;

    if(vm.count("bits"))
      bits = vm["bits"].as<int>();

    Glib::init();

    Dsp::Dsp dsp;
    Core::Core core(dsp.getControlApi());
    Audio::AlsaOut audioOut(dsp.getRealtimeApi(), vm["alsa-out"].as<std::string>(), bits);
    Ui::SharedState sharedUiState;
    Ui::Midi::Ui midiUI(sharedUiState, core.getApi());
    Ui::Touch::Ui touchUI(sharedUiState, core.getApi(), dsp.getDisplayApi());

    Ui::Midi::DebugUI dbg(sharedUiState, core.getApi(), dsp.getDisplayApi());
    dbg.build();
    touchUI.attach(dbg);
    touchUI.run();

    return EXIT_SUCCESS;
  }
  catch(const std::exception& e)
  {
    std::cout << "error parsing options: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
