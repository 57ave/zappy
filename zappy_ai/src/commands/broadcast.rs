use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use tokio::time::{sleep, Duration};

use crate::client::Position;
use crate::error::ClientError;

#[derive(Debug, Clone)]
pub struct BroadcastSystem {
    requests: Arc<Mutex<HashMap<u32, Vec<(String, Position)>>>>, 
}

impl BroadcastSystem {
    pub fn new() -> Self {
        Self {
            requests: Arc::new(Mutex::new(HashMap::new())),
        }
    }

    pub async fn handle_broadcast(&self, message: &str, sender_pos: Position) -> Option<Position> {
        let parts: Vec<&str> = message.split('|').collect();
        if parts.len() < 4 { return None; }

        match parts[0] {
            "HELP" => {
                let target_level = parts[1].parse().ok()?;
                let team = parts[2];
                let mut requests = self.requests.lock().unwrap();
                
                requests.entry(target_level)
                    .or_default()
                    .push((team.to_string(), sender_pos));
                
                None
            }
            "RESP" => {
                let target_level = parts[1].parse().ok()?;
                let team = parts[2];
                let requests = self.requests.lock().unwrap();
                
                if let Some(responders) = requests.get(&target_level) {
                    if responders.len() >= 2 {
                        return responders.iter()
                            .find(|(t, _)| t == team)
                            .map(|(_, pos)| *pos);
                    }
                }
                None
            }
            _ => None,
        }
    }
}
