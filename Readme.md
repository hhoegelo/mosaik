# TL;DR

* apt install gcc cmake make libboost-program-options-dev libasound2-dev libglibmm-2.4-dev libgtkmm-2.4-dev libgtkmm-3.0-dev libgstreamermm-1.0-dev nlohmann-json3-dev
* mkdir ~/mosaik-build
* cd ~/mosaik-build
* cmake path-to-source
* make -j12


# The big picture

The main() function connects 4 logical layers:

* core
* dsp
* AudioMidi
* ui

Communication between layers is done via interfaces with the following naming scheme:

```
namespace Layername /* for example 'Core' */
{
  namespace Api
  {
    namespace ScopeName /* for example Realtime, may be omitted, if the layer has only one single API to offer */
    {
      class Interface
      {
        /* 
          This class defines the interface to the API and has no implementation
          except to service the derived classes, for example it may offer some
          messaging or caching service.
        */
        

        virtual void doThisAndThat() = 0;
      };
      
      class Mosaik : public Interface 
      {
        /*
          Here, the real API is implemented
        */
        
        void doThisAndThat() override 
        {
          // do something
        }
      }
    }
  }
}
```

The extra 'Interface' class exists for supporting proper unit testing, so one layer can be tested against
all other layers mocked.

## Core

In core, the business logic of mosaik is implemented. All the stuff, that the user can change, 
e.g. volumes, samples, mute states, filter frequencies, is stored here in a big tree-like data 
structure. This data can be tweaked via the 'Core::Api::Mosaik' interface class. The value
scope should be 'user centric', so in Hertz, dB, percent etc.
When interface methods are invoked, they will most probably change some internal state of the data model
and finally they will calculate and prepare stuff for the 'dsp' layer and set this stuff via the 
Dsp::Api::Control::Interface. 

The Core layer can store and load the data model and by this provide undo/redo.

## Dsp

Parameters prepared by the core layer are set into the dsp layer via the Dsp::Api::Control::Mosaik, which
of course is a Dsp::Api::Control::Interface. The Dsp::Api::Control::Mosaik checks the incoming values and
set them directly in the dsp structures for the audio thread to pick them up in the next loop.

Dsp also has 'Dsp::Api::Realtime::Mosaik' (needless to say it as a 'Dsp::Api::Realtime::Interface') for the
AudioMidi implementation to connect to the dsp layer.