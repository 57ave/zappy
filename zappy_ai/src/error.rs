use std::io;
use std::fmt;
use std::error::Error;

#[derive(Debug)]
pub enum ClientError {
    IoError(io::Error),
    InvalidResponse(String),
    NoSlotsAvailable,
    ConnectionError(String),
    CommandError(String),
    ResourceError(String),
    InventoryError(String),
    IncantationError(String),
    BroadcastError(String),
}

impl Error for ClientError {}

impl fmt::Display for ClientError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            ClientError::IoError(e) => write!(f, "IO error: {}", e),
            ClientError::InvalidResponse(msg) => write!(f, "Invalid response: {}", msg),
            ClientError::NoSlotsAvailable => write!(f, "No slots available for team"),
            ClientError::ConnectionError(msg) => write!(f, "Connection error: {}", msg),
            ClientError::CommandError(msg) => write!(f, "Command error: {}", msg),
            ClientError::ResourceError(msg) => write!(f, "Resource error: {}", msg),
            ClientError::InventoryError(msg) => write!(f, "Inventory error: {}", msg),
            ClientError::IncantationError(msg) => write!(f, "Incantation error: {}", msg),
            ClientError::BroadcastError(msg) => write!(f, "Broadcast error: {}", msg),
        }
    }
}

impl From<io::Error> for ClientError {
    fn from(err: io::Error) -> Self {
        ClientError::IoError(err)
    }
}

// Méthodes utilitaires pour créer des erreurs
impl ClientError {
    pub fn invalid_response(msg: impl Into<String>) -> Self {
        ClientError::InvalidResponse(msg.into())
    }

    pub fn connection_error(msg: impl Into<String>) -> Self {
        ClientError::ConnectionError(msg.into())
    }

    pub fn command_error(msg: impl Into<String>) -> Self {
        ClientError::CommandError(msg.into())
    }

    pub fn resource_error(msg: impl Into<String>) -> Self {
        ClientError::ResourceError(msg.into())
    }

    pub fn inventory_error(msg: impl Into<String>) -> Self {
        ClientError::InventoryError(msg.into())
    }

    pub fn incantation_error(msg: impl Into<String>) -> Self {
        ClientError::IncantationError(msg.into())
    }

    pub fn broadcast_error(msg: impl Into<String>) -> Self {
        ClientError::BroadcastError(msg.into())
    }
} 