use crate::error::ClientError;

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub enum Resource {
    Food,
    Linemate,
    Deraumere,
    Sibur,
    Mendiane,
    Phiras,
    Thystame,
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


#[derive(Debug, Clone, Default)]
pub struct Inventory {
    pub food: i32,
    pub linemate: i32,
    pub deraumere: i32,
    pub sibur: i32,
    pub mendiane: i32,
    pub phiras: i32,
    pub thystame: i32,
}

impl Inventory {
    pub fn from_response(response: &str) -> Result<Self, ClientError> {
        let mut inv = Inventory::default();
        let cleaned = response.trim_matches(|c| c == '[' || c == ']');
        for entry in cleaned.split(',') {
            let parts: Vec<&str> = entry.trim().split_whitespace().collect();
            if parts.len() == 2 {
                let value = parts[1].parse::<i32>().unwrap_or(0);
                match parts[0] {
                    "food" => inv.food = value,
                    "linemate" => inv.linemate = value,
                    "deraumere" => inv.deraumere = value,
                    "sibur" => inv.sibur = value,
                    "mendiane" => inv.mendiane = value,
                    "phiras" => inv.phiras = value,
                    "thystame" => inv.thystame = value,
                    _ => {},
                }
            }
        }
        Ok(inv)
    }

    pub fn get_resource(&self, resource: &str) -> i32 {
        match resource {
            "food" => self.food,
            "linemate" => self.linemate,
            "deraumere" => self.deraumere,
            "sibur" => self.sibur,
            "mendiane" => self.mendiane,
            "phiras" => self.phiras,
            "thystame" => self.thystame,
            _ => 0,
        }
    }

    pub fn has_resource(&self, resource: &str) -> bool {
        self.get_resource(resource) > 0
    }

}