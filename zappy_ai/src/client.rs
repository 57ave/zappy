use tokio::net::TcpStream;
use tokio::io::{AsyncBufReadExt, AsyncWriteExt, BufReader, BufWriter};
use tokio::net::tcp::{OwnedReadHalf, OwnedWriteHalf};
use std::io;
use std::time::Duration;
use std::error::Error;
use crate::commands::commands::{Command, Resource, Direction};

pub struct ZappyClient {
    reader: BufReader<OwnedReadHalf>,
    writer: BufWriter<OwnedWriteHalf>,
    team_name: String,
    client_num: i32,
    map_width: usize,
    map_height: usize,
    freq: u32,
    debug: bool,
}

#[derive(Debug)]
pub enum ConnectionError {
    IoError(io::Error),
    InvalidResponse(String),
    NoSlotsAvailable,
}

impl Error for ConnectionError {}

impl std::fmt::Display for ConnectionError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ConnectionError::IoError(e) => write!(f, "IO error: {}", e),
            ConnectionError::InvalidResponse(s) => write!(f, "Invalid response: {}", s),
            ConnectionError::NoSlotsAvailable => write!(f, "No slots available"),
        }
    }
}

impl From<io::Error> for ConnectionError {
    fn from(err: io::Error) -> Self {
        ConnectionError::IoError(err)
    }
}

impl ZappyClient {
    pub async fn connect(host: &str, port: u16, team_name: &str, freq: u32, debug: bool) -> Result<Self, ConnectionError> {
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
            return Err(ConnectionError::InvalidResponse(format!("Expected WELCOME, got: {}", welcome.trim())));
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
            .map_err(|_| ConnectionError::InvalidResponse("Invalid client number".to_string()))?;
        
        if client_num <= 0 {
            return Err(ConnectionError::NoSlotsAvailable);
        }
        
        let mut dimensions = String::new();
        reader.read_line(&mut dimensions).await?;
        if debug {
            println!("Received: {}", dimensions.trim());
        }
        let dimensions: Vec<&str> = dimensions.trim().split_whitespace().collect();
        if dimensions.len() != 2 {
            return Err(ConnectionError::InvalidResponse("Invalid dimensions format".to_string()));
        }
        
        let map_width: usize = dimensions[0].parse()
            .map_err(|_| ConnectionError::InvalidResponse("Invalid width".to_string()))?;
        let map_height: usize = dimensions[1].parse()
            .map_err(|_| ConnectionError::InvalidResponse("Invalid height".to_string()))?;
        
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
        })
    }
    
    async fn read_response(&mut self) -> io::Result<String> {
        let mut response = String::new();
        self.reader.read_line(&mut response).await?;
        if self.debug {
            println!("Received: {}", response.trim());
        }
        Ok(response.trim().to_string())
    }
    
    pub async fn execute_command(&mut self, command: Command) -> io::Result<String> {
        let cmd_str = format!("{}\n", command.to_string());
        if self.debug {
            println!("Sent: {}", cmd_str.trim());
        }
        self.writer.write_all(cmd_str.as_bytes()).await?;
        self.writer.flush().await?;
        self.read_response().await
    }
    
    pub async fn forward(&mut self) -> io::Result<bool> {
        if self.debug {
            println!("Executing Forward command");
        }
        let response = self.execute_command(Command::Forward).await?;
        Ok(response == "ok")
    }
    
    pub async fn right(&mut self) -> io::Result<bool> {
        if self.debug {
            println!("Executing Right command");
        }
        let response = self.execute_command(Command::Right).await?;
        Ok(response == "ok")
    }
    
    pub async fn left(&mut self) -> io::Result<bool> {
        if self.debug {
            println!("Executing Left command");
        }
        let response = self.execute_command(Command::Left).await?;
        Ok(response == "ok")
    }
    
    pub async fn look(&mut self) -> io::Result<Vec<String>> {
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
    
    pub async fn inventory(&mut self) -> io::Result<Vec<(Resource, i32)>> {
        if self.debug {
            println!("Executing Inventory command");
        }
        let response = self.execute_command(Command::Inventory).await?;
        let inventory: Vec<(Resource, i32)> = response
            .trim_matches(|c| c == '[' || c == ']')
            .split(',')
            .filter_map(|s| {
                let parts: Vec<&str> = s.trim().split_whitespace().collect();
                if parts.len() == 2 {
                    if let (Some(resource), Ok(amount)) = (Resource::from_string(parts[0]), parts[1].parse()) {
                        Some((resource, amount))
                    } else {
                        None
                    }
                } else {
                    None
                }
            })
            .collect();
        if self.debug {
            println!("Inventory result: {:?}", inventory);
        }
        Ok(inventory)
    }
    
    pub async fn broadcast(&mut self, message: &str) -> io::Result<bool> {
        if self.debug {
            println!("Broadcasting message: {}", message);
        }
        let response = self.execute_command(Command::Broadcast(message.to_string())).await?;
        Ok(response == "ok")
    }
    
    pub async fn connect_nbr(&mut self) -> io::Result<i32> {
        if self.debug {
            println!("Executing ConnectNbr command");
        }
        let response = self.execute_command(Command::ConnectNbr).await?;
        let result = response.parse().map_err(|e| io::Error::new(io::ErrorKind::InvalidData, e))?;
        if self.debug {
            println!("ConnectNbr result: {}", result);
        }
        Ok(result)
    }
    
    pub async fn fork(&mut self) -> io::Result<bool> {
        if self.debug {
            println!("Executing Fork command");
        }
        let response = self.execute_command(Command::Fork).await?;
        Ok(response == "ok")
    }
    
    pub async fn eject(&mut self) -> io::Result<bool> {
        if self.debug {
            println!("Executing Eject command");
        }
        let response = self.execute_command(Command::Eject).await?;
        Ok(response == "ok")
    }
    
    pub async fn take(&mut self, resource: Resource) -> io::Result<bool> {
        if self.debug {
            println!("Taking resource: {:?}", resource);
        }
        let response = self.execute_command(Command::Take(resource)).await?;
        Ok(response == "ok")
    }
    
    pub async fn set(&mut self, resource: Resource) -> io::Result<bool> {
        if self.debug {
            println!("Setting resource: {:?}", resource);
        }
        let response = self.execute_command(Command::Set(resource)).await?;
        Ok(response == "ok")
    }
    
    pub async fn incantation(&mut self) -> io::Result<bool> {
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
}
