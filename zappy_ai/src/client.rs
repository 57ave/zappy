use tokio::net::TcpStream;
use tokio::io::{AsyncBufReadExt, AsyncWriteExt, BufReader, BufWriter};
use tokio::net::tcp::{OwnedReadHalf, OwnedWriteHalf};
use std::time::Duration;
use crate::commands::commands::Command;
use crate::drone::inventory::{Inventory, Resource};
use crate::error::ClientError;
use tokio::time::sleep;

#[derive(Debug)]
pub struct ZappyClient {
    reader: BufReader<OwnedReadHalf>,
    writer: BufWriter<OwnedWriteHalf>,
    team_name: String,
    client_num: i32,
    map_width: usize,
    map_height: usize,
    freq: u32,
    debug: bool,
    pub last_look: Option<Vec<String>>,
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


pub async fn see_food(&mut self) -> Result<u32, ClientError> {
    let tiles = self.get_look_cached().await?;
    Ok(tiles
        .iter()
        .filter(|tile| tile.contains("food"))
        .count() as u32)
}

pub async fn move_to_food(&mut self) -> Result<(), ClientError> {
    let tiles = self.get_look_cached().await?;
    if let Some((idx, _)) = tiles.iter().enumerate().find(|(_, t)| t.contains("food")) {
        if idx == 0 {
            self.take(Resource::Food).await?;
        } else {
            self.forward().await?;
        }
    } else {
        self.forward().await?;
    }
    Ok(())
}

pub async fn see_priority_resource(&mut self) -> Result<bool, ClientError> {
    let tiles = self.get_look_cached().await?;
    for tile in tiles {
        if tile.contains("linemate") || tile.contains("deraumere") || tile.contains("sibur") || tile.contains("phiras") {
            return Ok(true);
        }
    }
    Ok(false)
}

pub async fn move_to_resource(&mut self) -> Result<(), ClientError> {
    let tiles = self.get_look_cached().await?;
    if let Some((idx, tile)) = tiles.iter().enumerate().find(|(_, t)| t.contains("linemate") || t.contains("deraumere") || t.contains("sibur") || t.contains("phiras")) {
        if idx == 0 {
            if tile.contains("linemate") {
                self.take(Resource::Linemate).await?;
            } else if tile.contains("deraumere") {
                self.take(Resource::Deraumere).await?;
            } else if tile.contains("sibur") {
                self.take(Resource::Sibur).await?;
            } else if tile.contains("phiras") {
                self.take(Resource::Phiras).await?;
            }
        } else {
            self.forward().await?;
        }
    } else {
        self.forward().await?;
    }
    Ok(())
}

pub async fn has_level_requirements(&mut self) -> Result<bool, ClientError> {
    let inv = self.inventory().await?;
    Ok(inv.linemate >= 1)
}
}
