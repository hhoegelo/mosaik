# Mosaik Manual
![](logo.png)
Mosaik is a rhythm instrument, optimized for live beat programming. 

It combines techniques from sampling, stepsequcencing, live/jamming, looping, DJing, dubmixing to always give you full overview and control over your beats.

- sample sequence instrument
- made for stage

## Overview
- sampling
- stepsequcencing
- live/jamming: mute, mixer
- looping
- DJing: prelisten and crossfade
- dubmixing: mixer with fx-return on fadern

## UI
- tile grid, toolboxes
- dubmixer (channel, main)
![](new-ui.png)


## Toolboxes
![](toolboxes.png)

### new toolboxes
![](hwui-gui.png)

#### Audio Parameter
- Volume
- Pan
- Play Direction
- Pitch
- 2P HP Filter: cutoff, resonance
- BP Filter: gain, frequency, bandwidth
- 2P LP Filter: cutoff, resonace
- distortion (analog, saturator): gain, fold, asymmetry
- distotrion (digital, quantizer): step, offset, frequency
- filter and distortion curve display 

#### Waveform / Envelope
- Envelope: Start, FadeIn, FadeOut, End
- Hitpoint
- timeline
- barline
![](envelope.png)

#### StepWizzard
- set all
- clear all
- mirror
- invert 
- wizzard: rotate, gap, off 

![](step-wizzard.png)



#### Mute
- unmute all
- last selection
- groups
- stepmute

#### Snapshot
- slots: load, save

#### Browser
- prelisten sample
- browse sample
- bookmark samples
- unload sample from tile
- waveform
- load multiple tiles
- pathes: load/save

#### Prelisten
- headphones volume
- split or mixed
- swap left/right channel
- x-fade between main and pre

#### Manual
- shows the manual
- can be full page?

#### Audio Recorder
- select source: line in, mic in
- level
- save recording to sample collection
- load recording to tile

#### MIDI
- Key/Pad
- Controller: (Motor) Fader, Potis
- Rootnote
- MIDI Clock: internal, external

![](midi-assignment.png)

#### DMX
- Tile to DMX channel
- color

## Tiles
- Sample Player
- Volume, Pan
- VU meter
- Envelope, Hitpoint
- Pitch, Playdirection 
FX: Distortion, Filter

![](new-tiles.png)
- a) tile not playing
- b) tile triggered
- c) tile starts playing
- d) tile is playing
- e) tile is not playing
- f) tile with envelope, only envelope is shown, additional time for full sample length

#### Filter
- 2P HP: cutoff, resonance
- BP: gain, frequency, bandwidth 
- 2P LP: cutoff, resonance

#### Distortion
- Analog: Gain, Fold, Asymetry
- Digital: Step, Offset, Frequency

#### Reverb (Return Fader)


#### Delay (Return Fader)

## Concepts
- starts with empty tiles and steps and at random speed (60..180)
- prelisten
- automatic session recording and live sampling
- groups (mixer channel, mute, selection, steps?)
- undo
- load multiple tiles (no load/save)
- is normally in playmode (ext. midi?)

## OLD UI Overview

![](ui-overview.png)

### Quick Start Guide / Workflow
- 1) select a tile on the tile grid (4)
- 2) browse samples (2) via the controller and prelisten via headphones, the waveform will be shown in the Waveform Window (3)
- 3) load the selected sample to the selected tile via the controller,
- 4) set steps on the stepsequencer of the controller (the sequence can be prelisten via headphones)
- 5) use the fadelope to change the sample length (start, fade-in, fade-out, end) (3)
- 6) repeat the previous steps to get a full beat
- 7) change to mute overlay via the controller
    - select the tiles to be muted on the tile grid (4)
    - press unmute all (controller) to hear the full
     sequence again
    - press last mute to go back to the last mute states 


## Hardware
- 10inch touch display
- 64 step sequencer
- 16 software defined buttons
- 7 software defined encoder 

![](toppanel.png)

### IO-Panel
- frontpanel left: Headphones Out, Vol-Poti, USB
- frontpanel right: Line In, Mic In + Poti
- backpanel left: Main Out, Phono In, MIDI-Clock (In/Out)
- backpanel right: Power In, Power Switch, USB (MSD, MIDI, WIFI, BT), Ethernet (Internet, Ableton-Link), HDMI (mirror/extend), DMX out

![](connectors.png)


## Pages
- Main
- Manual
- Sample Organizer
- Audacity?
- Dubmixer?






