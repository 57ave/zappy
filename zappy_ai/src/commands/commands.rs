use serde::{Deserialize, Serialize};
use std::time::Duration;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum Direction {
    North,
    East,
    South,
    West,
}

#[derive(Debug, Clone, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum Resource {
    Food,
    Linemate,
    Deraumere,
    Sibur,
    Mendiane,
    Phiras,
    Thystame,
}

#[derive(Debug, Clone, PartialEq, Eq, Hash, Serialize, Deserialize)]
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

impl Resource {
    pub fn to_string(&self) -> String {
        match self {
            Resource::Food => "food",
            Resource::Linemate => "linemate",
            Resource::Deraumere => "deraumere",
            Resource::Sibur => "sibur",
            Resource::Mendiane => "mendiane",
            Resource::Phiras => "phiras",
            Resource::Thystame => "thystame",
        }.to_string()
    }

    pub fn from_string(s: &str) -> Option<Self> {
        match s {
            "food" => Some(Resource::Food),
            "linemate" => Some(Resource::Linemate),
            "deraumere" => Some(Resource::Deraumere),
            "sibur" => Some(Resource::Sibur),
            "mendiane" => Some(Resource::Mendiane),
            "phiras" => Some(Resource::Phiras),
            "thystame" => Some(Resource::Thystame),
            _ => None,
        }
    }
}

    