use crate::{client::Position, commands::commands::Direction, drone::inventory::Inventory};

#[derive(Debug, Clone)]
pub struct PlayerState {
    level: u32,
    position: Position,
    direction: Direction,
    inventory: Inventory,
}

impl PlayerState {
    pub fn new() -> Self {
        Self {
            level: 1,
            position: Position { x: 0, y: 0 },
            direction: Direction::Up,
            inventory: Inventory::default(),
        }
    }

    pub fn get_level(&self) -> u32 {
        self.level
    }

    pub fn set_level(&mut self, new_level: u32) {
        self.level = new_level;
    }

    pub fn get_direction(&self) -> Direction {
        self.direction
    }

    pub fn set_direction(&mut self, new_direction: Direction) {
        self.direction = new_direction;
    }

    pub fn get_position(&self) -> Position {
        self.position
    }

    pub fn set_position(&mut self, new_position: Position) {
        self.position = new_position;
    }

    pub fn can_level_up(&self) -> bool {
        self.level < 8
    }
}
