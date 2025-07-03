use tokio::net::TcpStream;
use tokio::io::{AsyncBufReadExt, AsyncWriteExt, BufReader, BufWriter};
use tokio::net::tcp::{OwnedReadHalf, OwnedWriteHalf};
use std::collections::{HashMap, VecDeque};
use std::future::Future;
use std::time::{Duration, Instant};
use crate::commands::{broadcast::BroadcastSystem, commands::{Command, Direction}};
use crate::drone::inventory::{Inventory, Resource};
use crate::drone::player_state::{self, PlayerState};
use crate::error::ClientError;
use tokio::time::sleep;
use crate::drone::levels;


#[derive(Debug)]
pub struct ZappyClient {
    reader: BufReader<OwnedReadHalf>,
    writer: BufWriter<OwnedWriteHalf>,
    pub team_name: String,
    client_num: i32,
    map_width: usize,
    map_height: usize,
    freq: u32,
    pub debug: bool,
    pub last_look: Option<Vec<String>>,
    pub player_state: PlayerState,
    broadcast: BroadcastSystem,
    pending_messages: VecDeque<String>,
    message_buffer: VecDeque<String>,
    last_message_time: Instant,
}

#[derive(Debug, Clone, Copy)]
pub struct Position {
    pub x: i32,
    pub y: i32,
}


impl ZappyClient {
    pub async fn connect(host: &str, port: u16, team_name: &str, freq: u32, debug: bool) -> Result<Self, ClientError> {
        if debug {
            println!("Connecting to {}:{}", host, port);
        }
        let stream = TcpStream::connect(format!("{}:{}", host, port)).await?;
        let (reader, writer) = stream.into_split();
        let mut reader = BufReader::new(reader);
        let mut writer = BufWriter::new(writer);
        
        let mut welcome = String::new();
        reader.read_line(&mut welcome).await?;
        if debug {
            println!("Received: {}", welcome.trim());
        }
        if !welcome.trim().eq("WELCOME") {
            return Err(ClientError::invalid_response(format!("Expected WELCOME, got: {}", welcome.trim())));
        }
        
        let team_msg = format!("{}\n", team_name);
        writer.write_all(team_msg.as_bytes()).await?;
        writer.flush().await?;
        if debug {
            println!("Sent: {}", team_msg.trim());
        }
        
        let mut client_num = String::new();
        reader.read_line(&mut client_num).await?;
        if debug {
            println!("Received: {}", client_num.trim());
        }
        let client_num: i32 = client_num.trim().parse()
            .map_err(|_| ClientError::invalid_response("Invalid client number"))?;
        
        if client_num <= 0 {
            return Err(ClientError::NoSlotsAvailable);
        }
        
        let mut dimensions = String::new();
        reader.read_line(&mut dimensions).await?;
        if debug {
            println!("Received: {}", dimensions.trim());
        }
        let dimensions: Vec<&str> = dimensions.trim().split_whitespace().collect();
        if dimensions.len() != 2 {
            return Err(ClientError::invalid_response("Invalid dimensions format"));
        }
        
        let map_width: usize = dimensions[0].parse()
            .map_err(|_| ClientError::invalid_response("Invalid width"))?;
        let map_height: usize = dimensions[1].parse()
            .map_err(|_| ClientError::invalid_response("Invalid height"))?;
        
        if debug {
            println!("Connected successfully. Map size: {}x{}", map_width, map_height);
        }
        Ok(ZappyClient {
            reader,
            writer,
            team_name: team_name.to_string(),
            client_num,
            map_width,
            map_height,
            freq,
            debug,
            last_look: None,
            player_state: PlayerState::new(),
            broadcast: BroadcastSystem::new(),
            pending_messages: std::collections::VecDeque::new(),
            message_buffer: std::collections::VecDeque::new(),
            last_message_time: std::time::Instant::now(),
        })
    }
    
    async fn read_response(&mut self) -> Result<String, ClientError> {
        let mut response = String::new();
        self.reader.read_line(&mut response).await?;
        if self.debug {
            println!("Received: {}", response.trim());
        }
        Ok(response.trim().to_string())
    }
    
    pub async fn execute_command(&mut self, command: Command) -> Result<String, ClientError> {
        let cmd_str = format!("{}\n", command.to_string());
        if self.debug {
            println!("Sent: {}", cmd_str.trim());
        }
        self.writer.write_all(cmd_str.as_bytes()).await?;
        self.writer.flush().await?;
        self.read_response().await
    }
    
    pub async fn forward(&mut self) -> Result<bool, ClientError> {
        if self.debug {
            println!("Executing Forward command");
        }
        let response = self.execute_command(Command::Forward).await?;
        Ok(response == "ok")
    }
    
    pub async fn right(&mut self) -> Result<bool, ClientError> {
        if self.debug {
            println!("Executing Right command");
        }
        let response = self.execute_command(Command::Right).await?;
        Ok(response == "ok")
    }
    
    pub async fn left(&mut self) -> Result<bool, ClientError> {
        if self.debug {
            println!("Executing Left command");
        }
        let response = self.execute_command(Command::Left).await?;
        Ok(response == "ok")
    }
    
    pub async fn look(&mut self) -> Result<Vec<String>, ClientError> {
        if self.debug {
            println!("Executing Look command");
        }
        let response = self.execute_command(Command::Look).await?;
        let tiles: Vec<String> = response
            .trim_matches(|c| c == '[' || c == ']')
            .split(',')
            .map(|s| s.trim().to_string())
            .collect();
        if self.debug {
            println!("Look result: {:?}", tiles);
        }
        Ok(tiles)
    }
    
    pub async fn inventory(&mut self) -> Result<Inventory, ClientError> {
        if self.debug {
            println!("Executing Inventory command");
        }
        let response = self.execute_command(Command::Inventory).await?;
        let inventory = Inventory::from_response(&response)?;
        if self.debug {
            println!("Inventory result: {:?}", inventory);
        }
        Ok(inventory)
    }
    
    pub async fn broadcast(&mut self, message: &str) -> Result<bool, ClientError> {
        if self.debug {
            println!("Broadcasting message: {}", message);
        }
        let response = self.execute_command(Command::Broadcast(message.to_string())).await?;
        Ok(response == "ok")
    }
    
    pub async fn connect_nbr(&mut self) -> Result<i32, ClientError> {
        if self.debug {
            println!("Executing ConnectNbr command");
        }
        let response = self.execute_command(Command::ConnectNbr).await?;
        let result = response.parse()
            .map_err(|_| ClientError::invalid_response("Invalid connect_nbr response"))?;
        if self.debug {
            println!("ConnectNbr result: {}", result);
        }
        Ok(result)
    }
    
    pub async fn fork(&mut self) -> Result<bool, ClientError> {
        if self.debug {
            println!("Executing Fork command");
        }
        let response = self.execute_command(Command::Fork).await?;
        Ok(response == "ok")
    }
    
    pub async fn eject(&mut self) -> Result<bool, ClientError> {
        if self.debug {
            println!("Executing Eject command");
        }
        let response = self.execute_command(Command::Eject).await?;
        Ok(response == "ok")
    }
    
    pub async fn take(&mut self, resource: Resource) -> Result<bool, ClientError> {
        if self.debug {
            println!("Taking resource: {:?}", resource);
        }
        let response = self.execute_command(Command::Take(resource)).await?;
        Ok(response == "ok")
    }
    
    pub async fn set(&mut self, resource: Resource) -> Result<bool, ClientError> {
        if self.debug {
            println!("Setting resource: {:?}", resource);
        }
        let response = self.execute_command(Command::Set(resource)).await?;
        Ok(response == "ok")
    }
    
    pub async fn incantation(&mut self) -> Result<bool, ClientError> {
        if self.debug {
            println!("Starting incantation");
        }
        let response = self.execute_command(Command::Incantation).await?;
        if self.debug {
            println!("Incantation response: {}", response);
        }
        Ok(response.starts_with("Elevation underway"))
    }
    
    pub fn get_map_dimensions(&self) -> (usize, usize) {
        (self.map_width, self.map_height)
    }
    
    pub fn get_client_num(&self) -> i32 {
        self.client_num
    }
    
    pub fn get_freq(&self) -> u32 {
        self.freq
    }

    pub async fn get_look_cached(&mut self) -> Result<&Vec<String>, ClientError> {
        if self.last_look.is_none() {
            let look = self.look().await?;
            self.last_look = Some(look);
        }
        Ok(self.last_look.as_ref().unwrap())
    }

    pub fn reset_look_cache(&mut self) {
        self.last_look = None;
    }

    pub async fn wait(&mut self) -> Result<(), ClientError> {
        sleep(Duration::from_millis(100)).await;
        Ok(())
    }


    pub async fn get_look_tile(&mut self, index: usize) -> Result<Vec<String>, ClientError> {
        let look_result = self.get_look_cached().await?;
        
        if index > 3 {
            return Err(ClientError::InvalidResponse("Tile index must be 0-3".to_string()));
        }

        let tile_str = look_result.get(index)
            .ok_or(ClientError::InvalidResponse("Not enough tiles in look response".to_string()))?;

        Ok(tile_str
            .split_whitespace()
            .filter(|s| !s.is_empty())
            .map(|s| s.to_string())
            .collect())
    }

pub async fn move_to_food(&mut self) -> Result<bool, ClientError> {
    let current_tile = self.get_look_tile(0).await?;
    if current_tile.contains(&"food".to_string()) {
        return Ok(true);
    }

    for idx in 1..=3 {
        let tile = self.get_look_tile(idx).await?;
        if tile.contains(&"food".to_string()) {
            match idx {
                1 => {
                    self.left().await?;
                    self.forward().await?;
                    self.right().await?;
                }
                2 => {
                    self.forward().await?;
                }
                3 => {
                    self.right().await?;
                    self.forward().await?;
                    self.left().await?;
                }
                _ => unreachable!(),
            }
            self.reset_look_cache();
            return Ok(true);
        }
    }

    self.forward().await?;
    self.reset_look_cache();
    Ok((false))
}

pub async fn move_to_resource(&mut self) -> Result<bool, ClientError> {
    const PRIORITY_RESOURCES: [(&str, Resource); 4] = [
        ("linemate", Resource::Linemate),
        ("deraumere", Resource::Deraumere),
        ("sibur", Resource::Sibur),
        ("phiras", Resource::Phiras),
    ];

    for i in 0..4 {
        let tile = self.get_look_tile(i).await?;
        for (resource_name, resource) in PRIORITY_RESOURCES {
            if tile.contains(&resource_name.to_string()) {
                match i {
                    0 => {
                        return Ok(true);
                    },
                    1 => {
                        self.left().await?;
                        self.forward().await?;
                        self.right().await?;
                    },
                    2 => {
                        self.forward().await?;
                    },
                    3 => {
                        self.right().await?;
                        self.forward().await?;
                        self.left().await?;
                    },
                    _ => (),
                }
                self.reset_look_cache();
            }
        }
    }

    self.forward().await?;
    Ok(false)
}

pub async fn take_all_resources(&mut self) -> Result<(), ClientError> {
    let current_tile = self.get_look_tile(0).await?;

    for resource_str in current_tile.iter() {
        if let Some(resource) = Resource::from_string(resource_str) {
            self.take(resource).await?;
        }
    }
    
    Ok(())
}

pub async fn see_food(&mut self) -> Result<u32, ClientError> {
    let mut food_count = 0;
    
    for i in 0..4 {
        food_count += self.get_look_tile(i).await?
            .iter()
            .filter(|&item| *item == "food".to_string())
            .count();
    }
    
    Ok(food_count as u32)
}

pub async fn see_priority_resource(&mut self) -> Result<bool, ClientError> {
    const PRIORITY_RESOURCES: [&str; 4] = ["linemate", "deraumere", "sibur", "phiras"];
    
    for i in 0..4 {
        if self.get_look_tile(i).await?
            .iter()
            .any(|item| PRIORITY_RESOURCES.contains(&item.as_str()))
        {
            return Ok(true);
        }
    }
    Ok(false)
}


//level

pub async fn has_level_requirements(&mut self) -> Result<bool, ClientError> {
    let current_level = self.get_level().await?;
    let requirements = levels::get_level_requirements()
        .into_iter()
        .find(|r| r.level == current_level + 1)
        .ok_or(ClientError::IncantationError("Wrong level".to_string()))?;

    let inventory = self.inventory().await?;
    let has_inventory_resources = requirements.resources.iter()
        .all(|(resource, &count)| inventory.get_resource(resource) >= count.try_into().unwrap());

    if !has_inventory_resources {
        return Ok(false);
    }

    let current_tile: Vec<String> = self.get_look_tile(0).await?;

    let mut tile_resources = HashMap::new();
    for resource in &current_tile {
        *tile_resources.entry(resource.to_string()).or_insert(0) += 1;
    }

    let has_tile_resources = requirements.resources.iter()
        .all(|(resource, count)| tile_resources.get(resource).unwrap_or(&0) >= count);

    if !has_tile_resources {
        return Ok(false);
    }

    let players_on_tile = current_tile.iter()
        .filter(|&r| r == "player")
        .count();

    Ok(players_on_tile >= requirements.required_players as usize)
}

pub async fn get_level(&mut self) -> Result<u32, ClientError> {
    Ok(self.player_state.get_level())
}

//position
pub async fn handle_level_up(&mut self) -> Result<(), ClientError> {
    if self.has_level_requirements().await? {
        self.incantation().await?;
        self.player_state.set_level(self.player_state.get_level() + 1);
    }
    Ok(())
}
pub async fn move_to_position(&mut self, target: Position) -> Result<(), ClientError> {
    let current = self.player_state.get_position();
    let delta = self.calculate_movement_delta(current, target).await?;

    if delta.x != 0 {
        self.face_direction(if delta.x > 0 { Direction::Right } else { Direction::Left }).await?;
        for _ in 0..delta.x.abs() {
            self.forward().await?;
        }
    }

    if delta.y != 0 {
        self.face_direction(if delta.y > 0 { Direction::Down } else { Direction::Up }).await?;
        for _ in 0..delta.y.abs() {
            self.forward().await?;
        }
    }

    Ok(())
}

async fn calculate_movement_delta(&self, current: Position, target: Position) -> Result<Position, ClientError> {
    let wrap_x = |x: i32| x.rem_euclid(self.map_width as i32);
    let wrap_y = |y: i32| y.rem_euclid(self.map_height as i32);

    let dx = wrap_x(target.x) - wrap_x(current.x);
    let dy = wrap_y(target.y) - wrap_y(current.y);

    let dx = if dx.abs() > self.map_width as i32 / 2 {
        dx - dx.signum() * self.map_width as i32
    } else {
        dx
    };

    let dy = if dy.abs() > self.map_height as i32 / 2 {
        dy - dy.signum() * self.map_height as i32
    } else {
        dy
    };

    Ok(Position { x: dx, y: dy })
}

async fn face_direction(&mut self, dir: Direction) -> Result<(), ClientError> {
    while self.player_state.get_direction() != dir {
        self.right().await?;
    }
    Ok(())
}

    pub async fn should_respond_to_help(&mut self, target_level: u32) -> Result<bool, ClientError> {
        let current_level = self.get_level().await?;
        Ok(current_level >= target_level.saturating_sub(1))
    }

    //broadcast

    pub async fn send_help_request(&mut self, target_level: u32) -> Result<(), ClientError> {
        let pos = self.player_state.get_position();
        let msg = format!("HELP|{}|{}|{}:{}", target_level, self.team_name, pos.x, pos.y);
        self.broadcast(&msg).await?;
        sleep(Self::BROADCAST_DELAY).await;
        Ok(())
    }

    pub async fn respond_to_help(&mut self, target_level: u32) -> Result<Option<Position>, ClientError> {
        let pos = self.player_state.get_position();
        let msg = format!("RESP|{}|{}|{}:{}", target_level, self.team_name, pos.x, pos.y);
        self.broadcast(&msg).await?;
        
        let start_time = std::time::Instant::now();
        while start_time.elapsed() < Self::BROADCAST_DELAY {
            if let Some(received) = self.check_messages().await? {
                if let Some(target_pos) = self.broadcast.handle_broadcast(&received, pos).await {
                    return Ok(Some(target_pos));
                }
            }
            sleep(Duration::from_millis(100)).await;
        }
        Ok(None)
    }

    const BROADCAST_DELAY: Duration = Duration::from_secs(2);
    const ACTION_DELAY: Duration = Duration::from_millis(100);

    pub async fn timed_action<F, Fut, R>(&mut self, action: F) -> Result<R, ClientError>
    where
        F: FnOnce(&mut Self) -> Fut,
        Fut: Future<Output = Result<R, ClientError>>,
    {
        let start = Instant::now();
        let result = action(self).await;
        let elapsed = start.elapsed();
        if elapsed < Self::ACTION_DELAY {
            sleep(Self::ACTION_DELAY - elapsed).await;
        }
        result
    }

    pub async fn coordinate_level_up(&mut self) -> Result<(), ClientError> {
        let current_level = self.get_level().await?;
        let requirements = levels::get_level_requirements()
            .into_iter()
            .find(|r| r.level == current_level + 1)
            .ok_or(ClientError::IncantationError("Max level reached".to_string()))?;

        let has_enough_food = self.inventory().await?.food > 5;

        if has_enough_food {
            let msg = format!("HELP|{}|{}|{}:{}", 
                current_level + 1,
                self.team_name,
                self.player_state.position.x,
                self.player_state.position.y
            );
            self.broadcast(&msg).await?;

            let start = Instant::now();
            let mut responders = 0;
            
            while start.elapsed() < Self::BROADCAST_DELAY {
                if let Some(message) = self.check_messages().await? {
                    if message.starts_with(&format!("RESP|{}", current_level + 1)) {
                        responders += 1;
                        if responders >= requirements.required_players - 1 {
                            break;
                        }
                    }
                }
                sleep(Duration::from_millis(50)).await;
            }

            if responders >= requirements.required_players - 1 {
                return self.handle_level_up().await;
            }
        }
        Ok(())
    }

    pub async fn process_broadcasts(&mut self) -> Result<(), ClientError> {
        let messages = self.check_messages().await?;
        for message in messages {
            self.broadcast.handle_broadcast(&message, self.player_state.get_position()).await;
        }
        Ok(())
    }
    pub async fn check_messages(&mut self) -> Result<Option<String>, ClientError> {
        if let Some(msg) = self.pending_messages.pop_front() {
            return Ok(Some(msg));
        }

        let mut buf = String::new();
        match self.reader.read_line(&mut buf).await {
            Ok(0) => Ok(None),
            Ok(_) => {
                let msg = buf.trim().to_string();
                if self.debug {
                    println!("[DEBUG] Received: {}", msg);
                }
                self.message_buffer.push_back(msg.clone());
                self.last_message_time = Instant::now();
                Ok(Some(msg))
            }
            Err(e) if e.kind() == std::io::ErrorKind::WouldBlock => Ok(None),
            Err(e) => Err(ClientError::IoError(e)),
        }
    }

    pub async fn does_need_help(&mut self, message: &str) -> Result<Option<(u32, Position)>, ClientError> {
        let parts: Vec<&str> = message.splitn(2, ',').collect();
        if parts.len() != 2 {
            return Ok(None);
        }

        let content = parts[1].trim();
        if !content.starts_with("HELP|") {
            return Ok(None);
        }

        let help_parts: Vec<&str> = content.split('|').collect();
        if help_parts.len() != 5 {
            return Ok(None);
        }

        if help_parts[2] != self.team_name {
            return Ok(None);
        }

        let target_level = help_parts[1].parse().unwrap_or(0);
        let x = help_parts[3].parse().unwrap_or(0);
        let y = help_parts[4].parse().unwrap_or(0);

        Ok(Some((target_level, Position { x, y })))
    }
}


