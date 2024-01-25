use crate::{Parameter, ParameterDiscriminants, ParameterTarget};
use signals2::{Connect1, Emit1, Signal};
use std::collections::HashMap;

struct CacheEntry {
    value: Parameter,
    signal: Signal<(Parameter,)>,
}

impl CacheEntry {
    pub fn new(p: &Parameter) -> Self {
        CacheEntry {
            value: p.clone(),
            signal: signals2::Signal::new(),
        }
    }
}

#[derive(Eq, Hash, PartialEq)]
struct CacheKey(ParameterTarget, ParameterDiscriminants);

pub struct Cache {
    entries: HashMap<CacheKey, CacheEntry>,
}

impl Cache {
    pub fn new() -> Self {
        Cache {
            entries: HashMap::new(),
        }
    }

    pub fn set(&mut self, target: ParameterTarget, p: Parameter) {
        let key = CacheKey(target, ParameterDiscriminants::from(&p));
        match self.entries.get_mut(&key) {
            None => {
                self.entries.insert(key, CacheEntry::new(&p));
            }
            Some(entry) => {
                entry.value = p;
                entry.signal.emit(entry.value.clone());
            }
        };
    }

    pub fn get(&self, target: ParameterTarget, p: ParameterDiscriminants) -> &Parameter {
        let key = CacheKey(target, p);
        &self
            .entries
            .get(&key)
            .expect("Cache not expected to be cold when being accessed")
            .value
    }

    pub fn connect<F>(
        &mut self,
        target: ParameterTarget,
        p: ParameterDiscriminants,
        cb: F,
    ) -> signals2::Connection 
    where F: Fn(Parameter) + Send + Sync + 'static
    {
        self.entries
            .get_mut(&CacheKey(target, p))
            .expect("Cache not expected to be cold when being accessed")
            .signal
            .connect(cb)
    }
}
