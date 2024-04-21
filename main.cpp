#include "dsp/Dsp.h"
#include "core/Core.h"
#include "audio/AlsaOut.h"
#include "ui/midi-ui/Ui.h"
#include "ui/touch-ui/Application.h"
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
    auto o = desc.add_options();
    o = o("help", "produce help message");
    o = o("alsa-out", value<std::string>(), "main device");
    o = o("bits", value<int>(), "16 or 32 bit");
    o = o("midi-ui", value<std::string>(), "alsa midi device for the mosaik hardware UI");
    o = o("channels", value<int>(), "number of audio channel: 2 or 4");

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

  std::string alsaOut = vm.count("alsa-out") ? vm["alsa-out"].as<std::string>() : "";
  std::string midiUi = vm.count("midi-ui") ? vm["midi-ui"].as<std::string>() : "";
  int bits = vm.count("bits") ? vm["bits"].as<int>() : 16;
  int channels = vm.count("channels") ? vm["channels"].as<int>() : 2;

  Glib::init();

  Dsp::Dsp dsp;
  Core::Core core(dsp.getControlApi(), Glib::MainContext::get_default());
  Audio::AlsaOut audioOut(dsp.getRealtimeApi(), alsaOut, bits, channels);
  Ui::Touch::Application touchUI(core.getApi(), dsp.getDisplayApi());
  Ui::Midi::Ui midiUI(midiUi, core.getApi(), dsp.getDisplayApi(), touchUI.getApi());
  Ui::Midi::DebugUI dbg(core.getApi(), dsp.getDisplayApi(), touchUI.getApi());

  std::filesystem::path home = getenv("HOME");
  auto storage = home / ".mosaik";

  core.getApi().load(storage);
  touchUI.attach(dbg);
  touchUI.run();
  core.getApi().save(storage);
  return EXIT_SUCCESS;
}
