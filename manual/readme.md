# Mosaik Manual

![](mosaik-logo.png)

Mosaik combines techniques from sampling, stepsequcencing, live/jamming, looping, DJing and dubmixing to always give you full overview and control over your beats.

- Mosaik - made for stage
- Mosaik is a rhythm instrument, optimized for live beat programming. 
- sample sequence instrument

![](mosaik-venn.png)

## Overview
- sampling
- stepsequcencing
- live/jamming: mute, mixer
- looping
- DJing: prelisten and crossfade
- dubmixing: mixer with fx-return on fadern

## Concepts
- starts with empty tiles and steps and at random speed (60..180)
- prelisten
- automatic session recording and live sampling
- groups (mixer channel, mute, selection, steps?)
- undo
- load multiple tiles (no load/save)
- is normally in playmode (ext. midi?)


## Hardware
- 10inch touch display
- 64 step sequencer
- 16 software defined buttons
- 7 software defined encoder 

![](toppanel.png)

## Funk-UI

![](funk-ui.png)

## Selection Frames

![](selection-frames.png)

## Software Defined HWUI

![](hwui-gui.png)

## Tiles
- Sample Player
- Volume, Pan
- VU meter
- Envelope, Hitpoint
- Pitch, Playdirection 
- FX: Distortion, Filter

![](new-tiles.png)

- a) tile not playing
- b) tile triggered
- c) tile starts playing
- d) tile is playing
- e) tile is not playing
- f) tile with envelope, only envelope is shown, additional time for full sample length



## Audio Parameter
- Volume
- Pan
- Play Direction
- Pitch
- filter and distortion curve display 

![](reaktor.png)

### Parameter Table

![](parameter-table.png)

## Audio Signal Flow

### Tiles
- top: minimal configuration: gain, hp, lp
- mid: medium configuration: gain, hp, peak-eq, lp
- bot: maximum configuration: gain, hp, distortion, peak-eq, lp

![](audio-tiles.png)

### Mixer

![](audio-dia-overview.png)

## Main/Mixer/Prelisten
- headphones volume
- split or mixed
- swap left/right channel
- x-fade between main and pre

![](prelisten.png)

## Envelope
- Envelope: Start, FadeIn, FadeOut, End
- move envelope
- Hitpoint
- timeline/barline

![](envelope.png)

## FX: Filter 

![](filter.png)

### 2P HiPass
- cutoff
- resonance

### Peak EQ (BP)
- gain
- frequency
- bandwidth 

### 2P LoPass
- cutoff
- resonance

## FX: Distortion

![](distortion.png)

### C15 Shaper (Analog)
- Gain
- Fold
- Asymetry
### Quantize Down Sample (Digital)
- Step
- Offset
- Frequency/Sample Rate

## FX: Reverb
Return Fader
- **Size**: room size
- **Color**: xxx
- **Pre Delay**: xxx
- **Chorus**: xxx

![](reverb.png)

## FX: Delay 
Return Fader
- Time
- Stereo
- Feedback
- Hi Cut
- Cross FB

![](delay.png)

## StepWizzard
- set all
- clear all
- mirror
- invert 
- wizzard: rotate, gap, off 

![](step-wizzard.png)


## Mute
- **unmute all**: saves mute selection to 'last selection' and unmutes all tiles
- **last selection**: recalls the last mute tile selection
- **groups**: mute groups are stored in slots
- **stepmute**: mutes all the trigger for the selected steps

![](mute.png)

## Snapshot
a snapshot includes all parameter and states of mosaik. snapshots can be stored in and recalled from slots.
- **take**: saves a snapshot to the selected slot
- **recall**: opens a snapshot from the selected slot 
- **1..6**: slots

![](snapshots.png)

## Browser
- prelisten sample
- browse sample
- bookmark samples
- unload sample from tile
- waveform
- load multiple tiles
- pathes: load/save

![](browser.png)

## Audio Recorder
- select source: line in, mic in
- level
- save recording to sample collection
- load recording to tile

## MIDI
- Key/Pad (Rootnote for tiles)
- Controller: (Motor) Fader, Potis
- MIDI Clock: internal, external
- MIDI learn/assign mode/overlay

![](midi-assignment.png)

![](midi-setup.png)

## DMX
- metronom
- visualize beat/bar (counter)
- link led-spots to tiles (trigger, color)
- show multiple tiles/sequneces on led-matrix

![](dmx.png)

## Manual
- shows the manual
- can be full page?

## Beat Book
- a way to store/learn rhythm pattern

## IO-Panel
- frontpanel left: Headphones Out, Vol-Poti, USB
- frontpanel right: Line In, Mic In + Poti
- backpanel left: Main Out, Phono In, MIDI-Clock (In/Out)
- backpanel right: Power In, Power Switch, USB (MSD, MIDI, WIFI, BT), Ethernet (Internet, Ableton-Link), HDMI (mirror/extend), DMX out

![](connectors.png)

## Electronics

![](electronics.jpeg)

## Pages
- Main
- Manual
- Sample Organizer
- Audacity?
- Dubmixer?



