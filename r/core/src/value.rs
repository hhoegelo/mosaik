use serde::Serialize;
use std::ops::Deref;

#[derive(Clone, Serialize)]
pub struct Value<
    const MIN_NOM: i32,
    const MIN_DEN: u32,
    const MAX_NOM: i32,
    const MAX_DEN: u32,
    const STEPS: u32,
>(f32);

impl<
        const MIN_NOM: i32,
        const MIN_DEN: u32,
        const MAX_NOM: i32,
        const MAX_DEN: u32,
        const STEPS: u32,
    > Value<MIN_NOM, MIN_DEN, MAX_NOM, MAX_DEN, STEPS>
{
    const MIN: f32 = MIN_NOM as f32 / MIN_DEN as f32;
    const MAX: f32 = MAX_NOM as f32 / MAX_DEN as f32;
    const RANGE: f32 = Self::MAX - Self::MIN;
    const INFLATE: f32 = STEPS as f32;
    const DEFLATE: f32 = 1f32 / STEPS as f32;
    const NORMALIZE: f32 = 1f32 / Self::RANGE;

    pub fn new(v: f32) -> Self {
        Self(Self::sanitize(v))
    }

    fn sanitize(v: f32) -> f32 {
        let start_from_zero = v - Self::MIN;
        let normalized = start_from_zero * Self::NORMALIZE;
        let inflated = normalized * Self::INFLATE;
        let rounded = inflated.round();
        let deflated = rounded * Self::DEFLATE;
        let denormalized = deflated * Self::RANGE;
        let back_to_min = denormalized + Self::MIN;
        let clamped = back_to_min.clamp(Self::MIN, Self::MAX);
        return clamped;
    }
}

impl<
        const MIN_NOM: i32,
        const MIN_DEN: u32,
        const MAX_NOM: i32,
        const MAX_DEN: u32,
        const STEPS: u32,
    > Deref for Value<MIN_NOM, MIN_DEN, MAX_NOM, MAX_DEN, STEPS>
{
    type Target = f32;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use float_cmp::approx_eq;

    #[test]
    fn value_bounds() {
        assert!(approx_eq!(
            f32,
            Value::<0, 1, 1, 1, 100>::new(0.4f32).0,
            0.4f32,
            ulps = 2
        ));

        assert!(approx_eq!(
            f32,
            Value::<0, 1, 1, 1, 100>::new(-0.4f32).0,
            0f32,
            ulps = 2
        ));

        assert!(approx_eq!(
            f32,
            Value::<0, 1, 1, 1, 100>::new(1.4f32).0,
            1f32,
            ulps = 2
        ));

        assert!(approx_eq!(
            f32,
            Value::<-10, 1, 10, 1, 10>::new(-0.4f32).0,
            0f32,
            ulps = 2
        ));

        assert!(approx_eq!(
            f32,
            Value::<-10, 1, 10, 1, 10>::new(-0.9f32).0,
            0f32,
            ulps = 2
        ));

        assert!(approx_eq!(
            f32,
            Value::<-10, 1, 10, 1, 10>::new(-1.2f32).0,
            -2f32,
            ulps = 2
        ));

        assert!(approx_eq!(
            f32,
            Value::<-10, 1, 10, 1, 10>::new(0.9f32).0,
            0f32,
            ulps = 2
        ));

        assert!(approx_eq!(
            f32,
            Value::<-10, 1, 10, 1, 10>::new(1.1f32).0,
            2f32,
            ulps = 2
        ));

        assert!(approx_eq!(
            f32,
            Value::<-3, 4, 3, 4, 100>::new(-0.8f32).0,
            -0.75f32,
            ulps = 2
        ));

        assert!(approx_eq!(
            f32,
            Value::<-3, 4, 3, 4, 100>::new(0.6f32).0,
            0.6,
            ulps = 2
        ));

        assert!(approx_eq!(
            f32,
            Value::<-3, 4, 3, 4, 100>::new(0.9f32).0,
            0.75,
            ulps = 2
        ));
    }

    #[test]
    fn value_steps() {
        let mut v: Value<0, 1, 1, 1, 100> = Value::new(0.4f32);
        assert!(approx_eq!(f32, v.0, 0.4f32, ulps = 2));
    }
}
