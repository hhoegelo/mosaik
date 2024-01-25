use crate::TileId;


pub trait Display {
    fn is_playing(&self, tile_id: TileId);
}
