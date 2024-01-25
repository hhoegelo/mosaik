mod parameter_cache;

use fixed::types::extra::U16;
use fixed::FixedI32;
use fixed::FixedU32;
use parameter_cache::Cache;
use std::rc::Rc;
use strum_macros::EnumDiscriminants;

type TileId = u8;

#[derive(EnumDiscriminants, Clone, Eq, Hash, PartialEq)]
#[strum_discriminants(derive(Hash))]
pub enum GlobalParameter {
    Tempo(FixedU32<U16>),
    Volume(FixedU32<U16>),
}

#[derive(EnumDiscriminants, Clone, Eq, Hash, PartialEq)]
#[strum_discriminants(derive(Hash))]
pub enum TileParameter {
    Selected(bool),
    SampleFile(Option<std::path::PathBuf>),
    Pattern([bool; 64]),
    Balance(FixedI32<U16>),
    Gain(FixedU32<U16>),
    Mute(bool),
    Reverse(bool),
}

struct Tile {
    selected: TileParameter,
    sample_file: TileParameter,
    pattern: TileParameter,
    balance: TileParameter,
    gain: TileParameter,
    mute: TileParameter,
    reverse: TileParameter,
}

pub struct Core {
    dsp: Rc<dyn dsp::Control>,

    tempo: GlobalParameter,
    volume: GlobalParameter,

    tiles: [Tile; 36],

    cache: Cache,
}

#[derive(Eq, Hash, PartialEq)]
pub enum ParameterTarget {
    Global,
    Tile(TileId),
}

impl Core {
    pub fn new(dsp: Rc<dyn dsp::Control>) -> Rc<Self> {
        Rc::new(Self {
            dsp: dsp,
            tempo: GlobalParameter::Tempo(FixedU32::from_num(120f32)),
            volume: GlobalParameter::Volume(FixedU32::from_num(1f32)),
            tiles: array_init::array_init(|_i| Tile::new()),
            cache: Cache::new(),
        })
    }

    pub fn set_tile_parameter(&mut self, target: TileId, parameter: TileParameter) {
        let tile = &mut self.tiles[target as usize];

        match parameter {
            a @ TileParameter::Selected(_) => tile.selected = a,
            a @ TileParameter::SampleFile(_) => tile.sample_file = a,
            a @ TileParameter::Pattern(_) => tile.pattern = a,
            a @ TileParameter::Balance(_) => tile.balance = a,
            a @ TileParameter::Gain(_) => tile.gain = a,
            a @ TileParameter::Mute(_) => tile.mute = a,
            a @ TileParameter::Reverse(_) => tile.reverse = a,
            _ => {}
        }

        //self.cache.set(target, parameter);
    }

    pub fn connect_parameter<F>(
        &mut self,
        target: TileId,
        parameter: TileParameterDiscriminants,
        cb: F,
    ) -> signals2::Connection
    where
        F: Fn(TileParameter) + Send + Sync + 'static,
    {
        self.cache.connect(target, parameter, cb)
    }
}

impl Default for Tile {
    fn default() -> Self {
        Self {
            selected: TileParameter::Selected(false),
            ..Default::default()
        }
    }
}

impl Tile {
    pub fn new() -> Self {
        Self {
            selected: TileParameter::Selected(false),
            sample_file: TileParameter::SampleFile(Option::None),
            pattern: TileParameter::Pattern(array_init::array_init(|_i| false)),
            balance: TileParameter::Balance(FixedI32::from_num(0f32)),
            gain: TileParameter::Gain(FixedU32::from_num(0f32)),
            mute: TileParameter::Mute(false),
            reverse: TileParameter::Reverse(false),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {}
}
