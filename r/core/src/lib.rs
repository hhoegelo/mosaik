mod value;

use std::ops::Deref;
use crate::value::Value;
use dsp::AudioKernel;
use serde::ser::SerializeTuple;
use serde::{Serialize, Serializer};
use signals2::{Connect1, Connection, Emit1, Signal};
use std::rc::Rc;
use enum_as_inner::EnumAsInner;
use strum_macros::EnumDiscriminants;

pub type TileId = u8;

#[derive(EnumDiscriminants, Clone, Serialize, EnumAsInner)]
pub enum GlobalParameter {
    Tempo(Value<20, 1, 240, 1, 1000>),
    Volume(Value<0, 1, 1, 1, 100>),
}

impl From<&GlobalParameter> for f32 {
    fn from(value: &GlobalParameter) -> Self {
        match value {
            GlobalParameter::Tempo(v) => { *(*v) }
            GlobalParameter::Volume(v) => { *(*v) }
        }
    }
}

#[derive(EnumDiscriminants, Clone, Serialize, EnumAsInner)]
pub enum TileParameter {
    Selected(bool),
    SampleFile(Option<std::path::PathBuf>),
    #[serde(serialize_with = "serialize")]
    Pattern([bool; 64]),
    Balance(Value<-1, 1, 1, 1, 200>),
    Gain(Value<0, 1, 1, 1, 100>),
    Mute(bool),
    Reverse(bool),
}

#[derive(Serialize)]
struct Signalling<P: Clone + 'static> {
    pub value: P,

    #[serde(skip_serializing)]
    pub signal: Signal<(P,)>,
}

impl<P: Clone + 'static> Signalling<P> {
    pub fn new(p: P) -> Self {
        Self {
            value: p,
            signal: Signal::new(),
        }
    }
}


#[derive(Serialize)]
struct Tile {
    selected: Signalling<TileParameter>,
    sample_file: Signalling<TileParameter>,
    pattern: Signalling<TileParameter>,
    balance: Signalling<TileParameter>,
    gain: Signalling<TileParameter>,
    mute: Signalling<TileParameter>,
    reverse: Signalling<TileParameter>,
}

#[derive(Serialize)]
pub struct Core {
    #[serde(skip_serializing)]
    dsp: Rc<dyn dsp::Control>,

    tempo: Signalling<GlobalParameter>,
    volume: Signalling<GlobalParameter>,

    #[serde(serialize_with = "serialize")]
    tiles: [Tile; 36],
}

#[derive(Eq, Hash, PartialEq)]
pub enum ParameterTarget {
    Global,
    Tile(TileId),
}

impl Core {
    pub fn new(dsp: Rc<dyn dsp::Control>) -> Rc<Self> {
        let r = Rc::new(Self {
            dsp,
            tempo: Signalling::new(GlobalParameter::Tempo(Value::new(120f32))),
            volume: Signalling::new(GlobalParameter::Volume(Value::new(1f32))),
            tiles: array_init::array_init(|_i| Tile::new()),
        });

        r.dsp.take_audio_kernel(r.as_ref().into());
        return r;
    }

    pub fn set_tile_parameter(&mut self, target: TileId, parameter: TileParameter) {
        Self::set_parameter(
            self.get_signaling_tile_parameter(target, (&parameter).into()),
            parameter,
        );
    }

    pub fn get_tile_parameter(
        &mut self,
        target: TileId,
        parameter: TileParameterDiscriminants,
    ) -> &TileParameter {
        &self.get_signaling_tile_parameter(target, parameter).value
    }

    pub fn connect_tile_parameter<F: Fn(TileParameter) + Send + Sync + 'static>(
        &mut self,
        target: TileId,
        parameter: TileParameterDiscriminants,
        cb: F,
    ) -> Connection {
        Self::connect_parameter(self.get_signaling_tile_parameter(target, parameter), cb)
    }

    pub fn set_global_parameter(&mut self, parameter: GlobalParameter) {
        Self::set_parameter(
            self.get_signaling_global_parameter((&parameter).into()),
            parameter,
        );
    }

    pub fn get_global_parameter(
        &mut self,
        parameter: GlobalParameterDiscriminants,
    ) -> &GlobalParameter {
        &self.get_signaling_global_parameter(parameter).value
    }

    pub fn connect_global_parameter<F: Fn(GlobalParameter) + Send + Sync + 'static>(
        &mut self,
        parameter: GlobalParameterDiscriminants,
        cb: F,
    ) -> Connection {
        Self::connect_parameter(self.get_signaling_global_parameter(parameter), cb)
    }

    fn connect_parameter<Parameter: Clone + 'static, F: Fn(Parameter) + Send + Sync + 'static>(
        target: &mut Signalling<Parameter>,
        cb: F,
    ) -> Connection {
        cb(target.value.clone());
        target.signal.connect(cb)
    }

    fn set_parameter<Parameter: Clone + 'static>(
        target: &mut Signalling<Parameter>,
        parameter: Parameter,
    ) {
        target.value = parameter.clone();
        target.signal.emit(parameter);
    }

    fn get_signaling_tile_parameter(
        &mut self,
        target: TileId,
        parameter: TileParameterDiscriminants,
    ) -> &mut Signalling<TileParameter> {
        let tile = &mut self.tiles[target as usize];

        match parameter {
            TileParameterDiscriminants::Selected => &mut tile.selected,
            TileParameterDiscriminants::SampleFile => &mut tile.sample_file,
            TileParameterDiscriminants::Pattern => &mut tile.pattern,
            TileParameterDiscriminants::Balance => &mut tile.balance,
            TileParameterDiscriminants::Gain => &mut tile.gain,
            TileParameterDiscriminants::Mute => &mut tile.mute,
            TileParameterDiscriminants::Reverse => &mut tile.reverse,
        }
    }

    fn get_signaling_global_parameter(
        &mut self,
        parameter: GlobalParameterDiscriminants,
    ) -> &mut Signalling<GlobalParameter> {
        match parameter {
            GlobalParameterDiscriminants::Tempo => &mut self.tempo,
            GlobalParameterDiscriminants::Volume => &mut self.volume,
        }
    }
}

impl Tile {
    pub fn new() -> Self {
        Self {
            selected: Signalling::new(TileParameter::Selected(false)),
            sample_file: Signalling::new(TileParameter::SampleFile(None)),
            pattern: Signalling::new(TileParameter::Pattern(array_init::array_init(|_i| false))),
            balance: Signalling::new(TileParameter::Balance(Value::new(0f32))),
            gain: Signalling::new(TileParameter::Gain(Value::new(1f32))),
            mute: Signalling::new(TileParameter::Mute(false)),
            reverse: Signalling::new(TileParameter::Reverse(false)),
        }
    }
}

impl From<&Core> for dsp::AudioKernel {

    fn from(core: &Core) -> Self {
        let num_frames_per_minute = 48000f32 * 60f32;
        let num16th_per_minute = core.tempo.value.as_tempo().unwrap() * 4;

        dsp::AudioKernel {
            volume: *(core.volume.value.as_volume().unwrap().deref()),
            frames_per16th: num_frames_per_minute / num16th_per_minute,
            /*target->volume = source.volume;

            auto r = std::make_unique<Dsp::AudioKernel>();
            translateGlobals(r, dataModel);

            for(auto c = 0; c < NUM_TILES; c++)
            {
            const auto &src = dataModel.tiles[c];
            auto &tgt = r->tiles[c];
            translateTile(dataModel, tgt, src);
         */


        };
        /*
                void translateGlobals(auto &target, const auto &source) const
                    {
                        auto num_frames_per_minute = SAMPLERATE * 60;
                        auto num16th_per_minute = source.tempo * 4;

                        target->frames_per16th = num_frames_per_minute / num16th_per_minute;
                        target->volume = source.volume;
                    }

                void translateTile(const DataModel &data, auto &tgt, const auto &src) const
                    {
                        tgt.pattern = src.pattern;
                        tgt.audio = m_dsp.getSamples(src.sample);
                        auto unbalancedGain = src.muted ? 0.f : src.gain;
                        tgt.gain_left = src.balance < 0 ? unbalancedGain : unbalancedGain * (1.0f - src.balance);
                        tgt.gain_right = src.balance > 0 ? unbalancedGain : unbalancedGain * (1.0f + src.balance);
                        tgt.playback_frame_increment = src.reverse ? -1 : 1;
                    }

         */
        todo!()
        }
    }
}

fn serialize<const N: usize, S, T>(t: &[T; N], serializer: S) -> Result<S::Ok, S::Error>
where
    S: Serializer,
    T: Serialize,
{
    let mut ser_tuple = serializer.serialize_tuple(N)?;
    for elem in t {
        ser_tuple.serialize_element(elem)?;
    }
    ser_tuple.end()
}
