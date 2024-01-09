#include "dsp/Dsp.h"
#include "core/Core.h"
#include "audio/AlsaOut.h"
#include "ui/midi-ui/Ui.h"
#include "ui/touch-ui/Ui.h"
#include "midi/Monitor.h"

#include <cxxopts.hpp>
#include <iostream>

int main(int args, char** argv)
{
  try
  {
    cxxopts::Options options(argv[0]);

    options.set_width(70).set_tab_expansion().allow_unrecognised_options().add_options()(
        "o,alsa-out", "Alsa Output Device", cxxopts::value<std::string>())("h,help", "Print usage");

    auto result = options.parse(args, argv);

    if(result.count("help"))
    {
      std::cout << options.help() << std::endl;
      return EXIT_SUCCESS;
    }

    std::cout << std::endl;

    Dsp::Dsp dsp;
    Core::Core core(dsp.getControlApi());
    Audio::AlsaOut audioOut(dsp.getRealtimeApi(), result["alsa-out"].as<std::string>());
    Ui::Midi::Ui midiUI(core.getApi(), dsp.getDisplayApi());
    Ui::Touch::Ui touchUI(core.getApi(), dsp.getDisplayApi());
    touchUI.run();

    return EXIT_SUCCESS;
  }
  catch(const cxxopts::exceptions::exception& e)
  {
    std::cout << "error parsing options: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
