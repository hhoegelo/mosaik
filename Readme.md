## TL;DR

requirements:

	apt install gcc cmake make libboost-program-options-dev libasound2-dev libglibmm-2.4-dev libgtkmm-2.4-dev libgtkmm-3.0-dev libgstreamermm-1.0-dev

setup and build:
	
	mkdir ~/mosaik-build
	cd ~/mosaik-build
	cmake path-to-source
	make

for release builds do:
	
 	mkdir ~/mosaik-build-release
	cd ~/mosaik-build-release
	cmake -DCMAKE_BUILD_TYPE=Release path-to-source
	make

start the app:

	./mosaik --alsa-out=plughw:1,0 --bits=24 --midi-ui hw:2,0,0 --channels 4
	



## HWUI debug tools
- aplay
- amidi
- aseqdump
- aconnect

## New Serial Protocol

brightness?

- **S**: stepsequencer buttons
- **M**: menu buttons
- **B**: encoder buttons
- **E**: encoder
- **L**: rgb(w) leds

set LED 102 to full green:
	
	L102:0:255:0
	
stepsequencer button 12 pressed:

	S12:1
	
stepsequencer button 12 released:

	S12:0


## RGB commands
'90 25 00'
cmd id val

commands:
90: red-channel (green and blue channels are uneffected)
91: green-channel (red and blue channels are uneffected)
92: blue-channel (red and green channels are uneffected)
93: white-channel (set rgb channels to the same value)
94: color table (choose from predefined colors)
95: update (initially leds where not set until an update cmd occured)
96: all leds on
97: all leds off (reset)

### RGB channel examples:
	
turn on red-channel of led 1 with brightness 5:

	amidi -p hw:1,0,0 -S '90 01 05'
	
turn on green-channel of led 1 with brightness 6:

	amidi -p hw:1,0,0 -S '91 01 06'

led 1 is yellow now, to show green, turn off the red channel:

	amidi -p hw:1,0,0 -S '90 01 00'
	
led 1 lights green now
 

### color map examples:

	# color table (94)       ID color
	amidi -p hw:1,0,0 -S '94 6A 00'
	{ 2, 0, 0}, // 0 red	relsub0
	{ 0, 2, 0}, // 1 green	relsub1
	{ 0, 0, 2}, // 2 blue	relsub2
	{ 2, 0, 2}, // 3 purple	relsub3
	{ 2, 2, 2}, // 4 white	stepled
	{ 0, 0, 0}, // 5 off  
	


#### Rpi DT Overlay for UARTs
bootfs/config.txt

	# enable usb hub
	dtoverlay=dwc2,dr_mode=host
	# otg_mode=1
	
	# enable all uarts
	dtoverlay=uart0
	dtoverlay=uart2
	dtoverlay=uart3
	dtoverlay=uart4
	dtoverlay=uart5
	
#### MIDI/DMX

- ttyMIDI
- openDMX
	
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
