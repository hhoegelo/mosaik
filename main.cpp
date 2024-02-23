#include "dsp/Dsp.h"
#include "core/Core.h"
#include "audio/AlsaOut.h"
#include "ui/midi-ui/Ui.h"
#include "ui/touch-ui/Ui.h"
#include "midi/Monitor.h"

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
    desc.add_options()("help", "produce help message")("alsa-out", value<std::string>(), "output device");

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

    Glib::init();

    Dsp::Dsp dsp;
    Core::Core core(dsp.getControlApi());
    Audio::AlsaOut audioOut(dsp.getRealtimeApi(), vm["alsa-out"].as<std::string>());
    Ui::Midi::Ui midiUI(core.getApi(), dsp.getDisplayApi());
    Ui::Touch::Ui touchUI(core.getApi(), dsp.getDisplayApi());
    touchUI.run();

    return EXIT_SUCCESS;
  }
  catch(const std::exception& e)
  {
    std::cout << "error parsing options: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
