N x UI (GTK)

---------------------------------

(API (C++-Calls): setParam(0..1), loadSample...)

Data (Persistenz)
json 

---------------------------------

(API: setParam(db))
Core (Channel)

^^^^^^^---------------------------------

VST / Audio / Midi (Jack)


----------------------

App:
main()
{
// nahezu keine abhaengigkeiten
Core core; <- inkl Midi Sync
Data data(core.getAPI());

AudioMidi am(core.getRTAPI());

N x MidiMosaikUI (ist eine UI) midi(data.getAPI(), core.getDisplayAPI());
MidiExternalControllerUI m2(data.getAPI(), core.getAPI());
GTKUI (ist eine weitere UI) ui(data.getAPI(), core.getAPI());
}


interface RTAPI{
  struct OutFrame {
    float mainLeft, mainRight;
    float preLeft, preRight;
  };

  struct InFrame {};
  
  virtual void doAudio(const InFrame *in, OutFrame *out, size_t numFrames, std::function<void (MidiEvent)> cb);
  virtual void doMidi(const MidiEvent &inEvent);
  
  
};
