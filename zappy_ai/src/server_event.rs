use std::str::FromStr;

use crate::drone::inventory::Inventory;

/// Parsing error from server detected
#[derive(Debug, Clone)]
pub struct ParseError {
    pub input: String,
    pub reason: &'static str,
}

impl ParseError {
    fn new(input: &str, reason: &'static str) -> Self {
        Self { input: input.to_string(), reason }
    }
}

#[derive(Debug, Clone)]
pub enum ServerEvent {
    Ok,
    Ko,
    Dead,
    ElevationUnderway,
    Message(String, String),
    Inventory(Inventory),
    Value(i32),
    Look(Vec<String>),
    Unknown(String),
}

impl FromStr for ServerEvent {
    type Err = ParseError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let input = s.trim();

        // Dispatch based on first "token" or distinctive character
        match Self::classify(input) {
            EventType::Simple => Self::parse_simple(input),
            EventType::Elevation => Ok(Self::ElevationUnderway),
            EventType::Message => Self::parse_message(input),
            EventType::Bracketed => Self::parse_bracketed(input),
            EventType::Numeric => Self::parse_numeric(input),
            EventType::Unknown => Ok(Self::Unknown(input.to_string())),
        }
    }
}

enum EventType {
    Simple,     // ok, ko, dead
    Elevation,  // Elevation underway
    Message,    // message K, text
    Bracketed,  // [...] (Look or Inventory)
    Numeric,    // number only
    Unknown,
}

impl ServerEvent {
    fn classify(input: &str) -> EventType {
        let lower = input.to_lowercase();
        if matches!(lower.as_str(), "ok" | "ko" | "dead") {
            return EventType::Simple;
        }
        
        if lower.starts_with("elevation") {
            return EventType::Elevation;
        }
        if lower.starts_with("message") {
            return EventType::Message;
        }
        
        if input.starts_with('[') && input.ends_with(']') {
            return EventType::Bracketed;
        }
        
        if input.chars().all(|c| c.is_ascii_digit() || c == '-') && !input.is_empty() {
            return EventType::Numeric;
        }
        
        EventType::Unknown
    }

    fn parse_simple(input: &str) -> Result<Self, ParseError> {
        match input.to_lowercase().as_str() {
            "ok" => Ok(Self::Ok),
            "ko" => Ok(Self::Ko),
            "dead" => Ok(Self::Dead),
            _ => Err(ParseError::new(input, "Expected ok/ko/dead")),
        }
    }

    fn parse_message(input: &str) -> Result<Self, ParseError> {
        let rest = input
            .strip_prefix("message")
            .or_else(|| input.strip_prefix("Message"))
            .unwrap_or(input)
            .trim();
        
        if let Some(comma_pos) = rest.find(',') {
            let direction = rest[..comma_pos].trim().to_string();
            let content = rest[comma_pos + 1..].trim().to_string();
            Ok(Self::Message(direction, content))
        } else {
            Ok(Self::Message("0".to_string(), rest.to_string()))
        }
    }

    fn parse_bracketed(input: &str) -> Result<Self, ParseError> {
        let content = input
            .trim_start_matches('[')
            .trim_end_matches(']')
            .trim();
        
        if content.is_empty() {
            return Ok(Self::Look(vec![]));
        }
        
        
        let segments: Vec<&str> = content.split(',').map(|s| s.trim()).collect();
        
        let is_inventory = segments.iter().all(|segment| {
            let words: Vec<&str> = segment.split_whitespace().collect();
            words.len() == 2 
                && words[0] != "player" 
                && words[1].parse::<i32>().is_ok()
        });
        
        if is_inventory {
            Inventory::from_response(input)
                .map(Self::Inventory)
                .map_err(|_| ParseError::new(input, "Failed to parse inventory"))
        } else {
            // C'est un Look - on split par virgule pour avoir chaque tile
            let tiles: Vec<String> = segments
                .iter()
                .map(|t| t.to_string())
                .filter(|t| !t.is_empty())
                .collect();
            Ok(Self::Look(tiles))
        }
    }

    fn parse_numeric(input: &str) -> Result<Self, ParseError> {
        input
            .parse::<i32>()
            .map(Self::Value)
            .map_err(|_| ParseError::new(input, "Failed to parse number"))
    }
}
