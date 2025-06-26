use serde::{Deserialize, Serialize};
use std::time::Duration;
use crate::drone::inventory::Resource;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum Direction {
    Up,
    Right,
    Down,
    Left,
}


#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub enum Command {
    Forward,
    Right,
    Left,
    Look,
    Inventory,
    Broadcast(String),
    ConnectNbr,
    Fork,
    Eject,
    Take(Resource),
    Set(Resource),
    Incantation,
}

impl Command {
    pub fn to_string(&self) -> String {
        match self {
            Command::Forward => "Forward".to_string(),
            Command::Right => "Right".to_string(),
            Command::Left => "Left".to_string(),
            Command::Look => "Look".to_string(),
            Command::Inventory => "Inventory".to_string(),
            Command::Broadcast(text) => format!("Broadcast {}", text),
            Command::ConnectNbr => "Connect_nbr".to_string(),
            Command::Fork => "Fork".to_string(),
            Command::Eject => "Eject".to_string(),
            Command::Take(resource) => format!("Take {}", resource.to_string()),
            Command::Set(resource) => format!("Set {}", resource.to_string()),
            Command::Incantation => "Incantation".to_string(),
        }
    }

    pub fn get_duration(&self, freq: u32) -> Duration {
        let base_time = match self {
            Command::Forward | Command::Right | Command::Left | Command::Look | 
            Command::Broadcast(_) | Command::Eject | Command::Take(_) | Command::Set(_) => 7,
            Command::Inventory => 1,
            Command::Fork => 42,
            Command::Incantation => 300,
            Command::ConnectNbr => 0,
        };
        Duration::from_secs_f64(base_time as f64 / freq as f64)
    }
}
    