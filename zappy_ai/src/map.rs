pub struct Map {
    width: usize,
    height: usize,
    tiles: Vec<Tile>,
}
pub struct Tile {
    pub element: Element,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum Element {
    Empty,
    Wall,
    Player,
    Enemy,
    Projectile,
    PowerUp,
    Explosion,
    Obstacle,
    Trap,
    Portal,
    Collectible,
    Hazard,
    Platform,
    Ladder,
    Door,
    Key,
    Chest,
    Spawner,
    Checkpoint,
    Finish,
}

impl Map {
    pub fn new(width: usize, height: usize) -> Self {
        Self {
            width,
            height,
            tiles: vec![Tile { element: Element::Empty }; width * height],
        }
    }
}