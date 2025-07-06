use std::collections::HashMap;

#[derive(Debug, Clone)]
pub struct LevelRequirement {
    pub level: u32,
    pub required_players: u32,
    pub resources: HashMap<String, u32>,
}

impl LevelRequirement {
    pub fn new(level: u32, required_players: u32, resources: Vec<(&str, u32)>) -> Self {
        let resources = resources.into_iter()
            .map(|(name, count)| (name.to_string(), count))
            .collect();
            
        Self {
            level,
            required_players,
            resources,
        }
    }
}

pub fn get_level_requirements() -> Vec<LevelRequirement> {
    vec![
        LevelRequirement::new(2, 1, vec![("linemate", 1)]),
        LevelRequirement::new(3, 2, vec![
            ("linemate", 1),
            ("deraumere", 1),
            ("sibur", 1),
        ]),
        LevelRequirement::new(4, 2, vec![
            ("linemate", 2),
            ("sibur", 1),
            ("phiras", 2),
        ]),
        LevelRequirement::new(5, 4, vec![
            ("linemate", 1),
            ("deraumere", 1),
            ("sibur", 2),
            ("phiras", 1),
        ]),
        LevelRequirement::new(6, 4, vec![
            ("linemate", 1),
            ("deraumere", 2),
            ("sibur", 1),
            ("mendiane", 3),
        ]),
        LevelRequirement::new(7, 6, vec![
            ("linemate", 1),
            ("deraumere", 2),
            ("sibur", 3),
            ("phiras", 1),
        ]),
        LevelRequirement::new(8, 6, vec![
            ("linemate", 2),
            ("deraumere", 2),
            ("sibur", 2),
            ("mendiane", 2),
            ("phiras", 2),
            ("thystame", 1),
        ]),
    ]
}