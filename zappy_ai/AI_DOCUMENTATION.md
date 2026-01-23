# 🤖 Zappy AI Client — Technical Documentation

> **An autonomous, async Rust client for the Zappy game protocol**

[![Rust](https://img.shields.io/badge/Rust-2021-orange?style=flat-square&logo=rust)](https://www.rust-lang.org/)
[![Tokio](https://img.shields.io/badge/Tokio-Async-blue?style=flat-square)](https://tokio.rs/)
[![License](https://img.shields.io/badge/License-EPITECH-green?style=flat-square)](LICENSE)

---

> ### 🔄 Recent Refactoring (January 2026)
> 
> This codebase has undergone a **major refactoring** to meet production-grade backend engineering standards:
> 
> - ✅ **Type-safe protocol parsing** with `ServerEvent` enum
> - ✅ **Race condition elimination** via single TCP reader pattern
> - ✅ **DRY command handling** with centralized execution
> - ✅ **Robust error handling** — no `unwrap()` in production paths
> 
> 📖 **[Read the full Refactoring Guide →](./REFACTORING.md)**

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Architecture](#-architecture)
- [Quick Start](#-quick-start)
- [Core Components](#-core-components)
- [Server Event Parsing](#-server-event-parsing)
- [Decision System](#-decision-system)
- [Commands Reference](#-commands-reference)
- [Resources & Inventory](#-resources--inventory)
- [Level Requirements](#-level-requirements)
- [Communication Protocol](#-communication-protocol)
- [Error Handling](#-error-handling)
- [Debug Mode](#-debug-mode)
- [Best Practices](#-best-practices)

---

## 🎯 Overview

The Zappy AI is a **production-grade autonomous client** that connects to the Zappy game server and plays the game using intelligent decision-making. Built with modern Rust practices, it features:

### ✨ Key Features

| Feature | Description |
|---------|-------------|
| **🔄 Async I/O** | Non-blocking TCP with Tokio runtime |
| **🧠 Decision Tree AI** | Priority-based action selection |
| **🔒 Type-Safe Protocol** | `ServerEvent` enum for all server responses |
| **🛡️ Race-Condition Free** | Single TCP reader pattern |
| **📡 Team Coordination** | Broadcast-based communication |
| **⚡ Zero-Copy Parsing** | Efficient response handling |

### Design Principles

1. **Single Point of TCP Reading** — All network reads go through `read_response_filtered()`, preventing race conditions
2. **Type-Safe Event Handling** — Server responses are parsed into `ServerEvent` enum variants
3. **DRY Command Execution** — Centralized `execute_command()` method for all operations
4. **Graceful Error Handling** — No `unwrap()` in production paths, comprehensive `ClientError` enum

---

## 🏗 Architecture

```
zappy_ai/
├── Cargo.toml                  # Dependencies & project config
├── Makefile                    # Build automation
├── AI_DOCUMENTATION.md         # This file
└── src/
    ├── main.rs                 # Entry point, CLI parsing (clap)
    ├── client.rs               # ZappyClient: TCP client & commands
    ├── server_event.rs         # ServerEvent enum & parsing
    ├── error.rs                # ClientError enum
    ├── mod.rs                  # Module re-exports
    │
    ├── commands/
    │   ├── mod.rs              # Module declarations
    │   ├── commands.rs         # Command enum & Direction
    │   └── broadcast.rs        # BroadcastSystem for team comms
    │
    ├── decision/
    │   ├── mod.rs              # Priority & Action enums
    │   ├── decision_tree.rs    # FoodNode, ResourceNode, LevelUpNode
    │   └── decision_caller.rs  # Main loop & action handlers
    │
    └── drone/
        ├── mod.rs              # Module declarations
        ├── inventory.rs        # Inventory struct & Resource enum
        ├── levels.rs           # LevelRequirement struct
        ├── player_state.rs     # PlayerState tracking
        └── providers.rs        # Resource collection helpers
```

### Module Dependency Graph

```
main.rs
    │
    ▼
┌─────────────────────────────────────────────────────┐
│                    client.rs                         │
│  ┌─────────────┐  ┌──────────────┐  ┌────────────┐  │
│  │ TCP Reader  │──│ ServerEvent  │──│ Commands   │  │
│  └─────────────┘  └──────────────┘  └────────────┘  │
└─────────────────────────────────────────────────────┘
                          │
          ┌───────────────┼───────────────┐
          ▼               ▼               ▼
    ┌──────────┐   ┌──────────┐   ┌──────────────┐
    │ decision │   │  drone   │   │   commands   │
    └──────────┘   └──────────┘   └──────────────┘
```

---

## 🚀 Quick Start

### Prerequisites

- **Rust** 1.70+ (2021 edition)
- **Cargo** (included with Rust)

### Build

```bash
cd zappy_ai
make           # or: cargo build --release
```

### Run

```bash
# Minimal
./zappy_ai -p 4242 -n team1

# Full options
./zappy_ai -p 4242 -n team1 -m localhost -d

# Using cargo
cargo run -- -p 4242 -n team1 -d
```

### CLI Arguments

| Arg | Long | Default | Required | Description |
|-----|------|---------|----------|-------------|
| `-p` | `--port` | `4242` | No | Server port |
| `-n` | `--name` | — | **Yes** | Team name |
| `-m` | `--machine` | `localhost` | No | Server host |
| `-d` | `--debug` | `false` | No | Enable debug logs |

---

## 🧩 Core Components

### ZappyClient Structure

```rust
pub struct ZappyClient {
    // ─── Network I/O ───────────────────────────────────
    reader: BufReader<OwnedReadHalf>,    // Single TCP reader
    writer: BufWriter<OwnedWriteHalf>,   // Buffered writer
    
    // ─── Game State ────────────────────────────────────
    pub team_name: String,
    map_width: usize,
    map_height: usize,
    freq: u32,                           // Server tick frequency
    
    // ─── Caching & State ───────────────────────────────
    pub last_look: Option<Vec<String>>,  // Look cache
    pub player_state: PlayerState,       // Level, position, direction
    
    // ─── Communication ─────────────────────────────────
    broadcast: BroadcastSystem,
    pending_messages: VecDeque<String>,  // Queued broadcasts
    
    // ─── Debug ─────────────────────────────────────────
    pub debug: bool,
}
```

### Connection Handshake

```
┌────────┐                              ┌────────┐
│ Client │                              │ Server │
└───┬────┘                              └───┬────┘
    │                                       │
    │◄──────────── WELCOME ─────────────────│
    │                                       │
    │────────── team_name\n ───────────────►│
    │                                       │
    │◄───────── client_slots ───────────────│
    │◄────────── width height ──────────────│
    │                                       │
    │         [ Game Loop Starts ]          │
```

### Key Methods

| Method | Signature | Description |
|--------|-----------|-------------|
| `connect` | `async fn connect(...) -> Result<Self>` | Establish connection |
| `execute_command` | `async fn execute_command(&mut self, cmd) -> Result<ServerEvent>` | Send any command |
| `read_response_filtered` | `async fn read_response_filtered(&mut self) -> Result<ServerEvent>` | **Single TCP read point** |
| `look` | `async fn look(&mut self) -> Result<Vec<String>>` | Get visible tiles |
| `inventory` | `async fn inventory(&mut self) -> Result<Inventory>` | Get inventory |
| `forward` / `left` / `right` | `async fn ...(&mut self) -> Result<bool>` | Movement |
| `take` / `set` | `async fn ...(&mut self, resource) -> Result<bool>` | Resource handling |
| `broadcast` | `async fn broadcast(&mut self, msg) -> Result<bool>` | Team communication |

---

## 🔍 Server Event Parsing

### The Race Condition Problem (Solved)

**Problem:** Multiple async tasks reading from TCP can receive each other's responses.

**Solution:** A single `read_response_filtered()` method handles ALL TCP reads:

```rust
async fn read_response_filtered(&mut self) -> Result<ServerEvent, ClientError> {
    loop {
        let response = self.read_response().await?;
        let event = self.parse_event(&response)?;
        
        match event {
            // Queue broadcast messages for later processing
            ServerEvent::Message(_, _) => {
                self.pending_messages.push_back(response);
                continue;
            },
            // Handle death immediately
            ServerEvent::Dead => {
                return Err(ClientError::ConnectionError("Player is dead".to_string()));
            },
            // Return all other events
            _ => return Ok(event),
        }
    }
}
```

### ServerEvent Enum

```rust
pub enum ServerEvent {
    Ok,                           // Command succeeded
    Ko,                           // Command failed
    Dead,                         // Player died
    ElevationUnderway,            // Incantation started
    Message(String, String),      // (direction, content)
    Inventory(Inventory),         // Parsed inventory
    Value(i32),                   // Numeric response (Connect_nbr)
    Look(Vec<String>),            // Tile contents
    Unknown(String),              // Unrecognized response
}
```

### Parsing Strategy

The parser uses a **classification system** for efficient dispatch:

```rust
impl FromStr for ServerEvent {
    fn from_str(s: &str) -> Result<Self, ParseError> {
        match Self::classify(input) {
            EventType::Simple    => Self::parse_simple(input),    // ok/ko/dead
            EventType::Elevation => Ok(Self::ElevationUnderway),
            EventType::Message   => Self::parse_message(input),   // message K, text
            EventType::Bracketed => Self::parse_bracketed(input), // Look or Inventory
            EventType::Numeric   => Self::parse_numeric(input),   // numbers
            EventType::Unknown   => Ok(Self::Unknown(input.to_string())),
        }
    }
}
```

### Look vs Inventory Disambiguation

Both responses are bracketed `[...]`. The parser distinguishes them by checking if **all segments** match the inventory pattern (`name count`):

```rust
// Inventory: [food 10, linemate 2, deraumere 0, sibur 1, ...]
// Look:      [player food, linemate, , sibur]

let is_inventory = segments.iter().all(|segment| {
    let words: Vec<&str> = segment.split_whitespace().collect();
    words.len() == 2 
        && words[0] != "player" 
        && words[1].parse::<i32>().is_ok()
});
```

---

## 🧠 Decision System

### Priority Levels

```rust
pub enum Priority {
    Critical,  // Execute immediately (death imminent, level-up ready)
    High,      // Execute soon (low food, resources nearby)
    Medium,    // Normal priority
    Low,       // Can be deferred (exploration)
}
```

### Available Actions

```rust
pub enum Action {
    FindFood,         // Search and collect food
    CollectResource,  // Gather level-up resources
    LevelUp,          // Start incantation
    LayEgg,           // Fork (create spawn point)
    JoinTeam,         // Respond to help request
    MaintainFood,     // Keep food above threshold
    Explore,          // Random movement
    Wait,             // Idle
}
```

### Decision Tree Structure

```
                    ┌─────────────────┐
                    │   FoodNode      │
                    │ threshold: 4    │
                    └────────┬────────┘
                             │
               ┌─────────────┴─────────────┐
               │ food < 4?                 │
               ▼                           ▼
    ┌──────────────────┐        ┌─────────────────┐
    │ (High, FindFood) │        │  ResourceNode   │
    └──────────────────┘        └────────┬────────┘
                                         │
                          ┌──────────────┴──────────────┐
                          │ has level 4 resources?      │
                          ▼                             ▼
               ┌────────────────────┐       ┌─────────────────┐
               │(Critical, LevelUp) │       │  LevelUpNode    │
               └────────────────────┘       └────────┬────────┘
                                                     │
                                                     ▼
                                          ┌─────────────────┐
                                          │    JoinNode     │
                                          └─────────────────┘
```

### Decision Flow Table

| Node | Condition | Priority | Action |
|------|-----------|----------|--------|
| **FoodNode** | `food < threshold` | High | FindFood |
| **FoodNode** | `food >= threshold` | — | *pass to next* |
| **ResourceNode** | Has all resources for level 4+ | Critical | LevelUp |
| **ResourceNode** | `food > 8` & needs resources | High | CollectResource |
| **LevelUpNode** | Can coordinate elevation | Critical | LevelUp |
| **JoinNode** | Help request received | High | JoinTeam |
| **Default** | Nothing urgent | Low | Explore / Wait |

### Main Decision Loop

```rust
pub async fn make_decision(client: &mut ZappyClient) -> Result<(), ClientError> {
    let decision_tree = DecisionTree::new();

    loop {
        let (priority, action) = decision_tree.evaluate(client).await;
        
        match (priority, action) {
            (Priority::Critical, Action::LevelUp) => handle_level_up(client).await?,
            (Priority::High, Action::FindFood) => handle_food_search(client).await?,
            (Priority::High, Action::CollectResource) => handle_resource_collection(client).await?,
            (Priority::High, Action::LayEgg) => handle_lay_egg(client).await?,
            (Priority::High, Action::JoinTeam) => handle_join_team(client).await?,
            (Priority::Low, Action::Explore) => handle_exploration(client).await?,
            _ => client.wait().await?,
        }
        
        client.process_broadcasts().await?;
        client.reset_look_cache();
    }
}
```

---

## ⚙️ Commands Reference

### Command Enum

```rust
pub enum Command {
    Forward,              // Move one tile forward
    Right,                // Rotate 90° clockwise
    Left,                 // Rotate 90° counter-clockwise
    Look,                 // View surrounding tiles
    Inventory,            // Check inventory
    Broadcast(String),    // Send message to all
    ConnectNbr,           // Get available team slots
    Fork,                 // Lay egg (create spawn)
    Eject,                // Push players off tile
    Take(Resource),       // Pick up resource
    Set(Resource),        // Drop resource
    Incantation,          // Start elevation ritual
}
```

### Command Timing

| Command | Ticks | At freq=100 |
|---------|-------|-------------|
| `Forward` | 7 | 70ms |
| `Right` / `Left` | 7 | 70ms |
| `Look` | 7 | 70ms |
| `Inventory` | 1 | 10ms |
| `Broadcast` | 7 | 70ms |
| `Take` / `Set` | 7 | 70ms |
| `Eject` | 7 | 70ms |
| `Fork` | 42 | 420ms |
| `Incantation` | 300 | 3000ms |
| `Connect_nbr` | 0 | instant |

**Formula:** `duration = ticks / frequency`

### Look Command Response

```
Look returns tiles in this order (level 1):

         ┌───┐
         │ 1 │
         └───┘
    ┌───┬───┬───┐
    │ 2 │ 3 │ 4 │
    └───┴───┴───┘
        ┌───┐
        │ 0 │  ← Player position
        └───┘
          ▲
      Facing direction
```

Response format: `[tile0, tile1, tile2, tile3, ...]`

---

## 📦 Resources & Inventory

### Resource Enum

```rust
pub enum Resource {
    Food,       // Survival (1 food = 126 time units)
    Linemate,   // Common stone
    Deraumere,  // Uncommon stone
    Sibur,      // Uncommon stone
    Mendiane,   // Rare stone
    Phiras,     // Rare stone
    Thystame,   // Very rare stone
}
```

### Inventory Structure

```rust
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
```

### Parsing

```rust
// Server response: [food 10, linemate 2, deraumere 0, sibur 1, ...]
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
                    // ... other resources
                    _ => {},
                }
            }
        }
        Ok(inv)
    }
}
```

---

## 📊 Level Requirements

### Elevation Table

| Level | Players | linemate | deraumere | sibur | mendiane | phiras | thystame |
|-------|---------|----------|-----------|-------|----------|--------|----------|
| 1 → 2 | 1 | 1 | 0 | 0 | 0 | 0 | 0 |
| 2 → 3 | 2 | 1 | 1 | 1 | 0 | 0 | 0 |
| 3 → 4 | 2 | 2 | 0 | 1 | 0 | 2 | 0 |
| 4 → 5 | 4 | 1 | 1 | 2 | 0 | 1 | 0 |
| 5 → 6 | 4 | 1 | 2 | 1 | 3 | 0 | 0 |
| 6 → 7 | 6 | 1 | 2 | 3 | 0 | 1 | 0 |
| 7 → 8 | 6 | 2 | 2 | 2 | 2 | 2 | 1 |

### Level-Up Process

```
1. Collect required resources
2. Find/gather required players on same tile
3. Drop resources on tile
4. Start incantation
5. Wait for "Elevation underway"
6. Wait for "Current level: N"
```

---

## 📡 Communication Protocol

### Server Response Types

| Response | Meaning | ServerEvent |
|----------|---------|-------------|
| `WELCOME` | Connection accepted | *(handshake)* |
| `ok` | Command succeeded | `Ok` |
| `ko` | Command failed | `Ko` |
| `dead` | Player starved | `Dead` |
| `message K, text` | Broadcast received | `Message(K, text)` |
| `Elevation underway` | Ritual started | `ElevationUnderway` |
| `Current level: N` | Level up success | *(parsed separately)* |
| `[...]` | Look or Inventory | `Look(...)` / `Inventory(...)` |
| `N` | Numeric (Connect_nbr) | `Value(N)` |

### Broadcast Directions

```
          ┌───┐
          │ 1 │
          └───┘
     ┌───┐     ┌───┐
     │ 8 │     │ 2 │
     └───┘     └───┘
┌───┐     ┌───┐     ┌───┐
│ 7 │     │ 0 │     │ 3 │
└───┘     └───┘     └───┘
     ┌───┐     ┌───┐
     │ 6 │     │ 4 │
     └───┘     └───┘
          ┌───┐
          │ 5 │
          └───┘

0 = Same tile
1-8 = Direction relative to player facing
```

### Team Coordination Protocol

```
Help Request:  HELP|<target_level>|<team_name>|<x>:<y>
Response:      RESP|<target_level>|<team_name>|<x>:<y>
```

---

## 🛡️ Error Handling

### ClientError Enum

```rust
pub enum ClientError {
    IoError(io::Error),           // Network/file errors
    InvalidResponse(String),       // Protocol violation
    NoSlotsAvailable,             // Team is full
    ConnectionError(String),       // Connection failed/lost
    CommandError(String),          // Command execution failed
    ResourceError(String),         // Invalid resource
    InventoryError(String),        // Inventory parsing failed
    IncantationError(String),      // Level-up failed
    BroadcastError(String),        // Communication error
}
```

### Error Recovery

| Error Type | Recovery Strategy |
|------------|-------------------|
| `IoError` | Reconnect or exit |
| `InvalidResponse` | Log & continue |
| `NoSlotsAvailable` | Exit gracefully |
| `Dead` | Exit (game over) |

---

## 🐛 Debug Mode

Enable with `-d` flag:

```bash
./zappy_ai -p 4242 -n team1 -d
```

### Example Output

```
Connecting to localhost:4242
Received: WELCOME
Sent: team1
Received: 5
Received: 20 20
Connected successfully. Map size: 20x20
Executing Look command
Sent: Look
Received: ko
Ignoring stale ok/ko response
Received: [food 46 linemate 20 sibur 7 player 1,food 87 linemate 60...]
Look result: ["food 46 linemate 20 sibur 7 player 1", ...]
Executing Inventory command
Sent: Inventory
Received: [food 34, linemate 39, deraumere 9, sibur 18, ...]
```

---

## ✅ Best Practices

### For Contributors

1. **Never use `unwrap()` in production code** — use `?` or explicit error handling
2. **All TCP reads through `read_response_filtered()`** — maintains single-reader invariant
3. **Parse responses into `ServerEvent`** — no raw string comparisons in business logic
4. **Use `execute_command()` for all commands** — ensures consistent send/receive pattern
5. **Reset look cache after movement** — prevents stale data

### Code Style

```rust
// ✅ Good: Type-safe event handling
match self.execute_command(Command::Forward).await? {
    ServerEvent::Ok => Ok(true),
    ServerEvent::Ko => Ok(false),
    _ => Err(ClientError::invalid_response("Expected ok/ko")),
}

// ❌ Bad: String comparison
if response == "ok" { ... }
```

---

## 📚 See Also

- [Server Documentation](../documentation/DEV_DOCUMENTATION_ZAPPY.txt)
- [User Manual](../documentation/USER_DOCUMENTATION_ZAPPY.txt)
- [Cargo.toml](./Cargo.toml) — Dependencies

---

<div align="center">

**Part of the Zappy Project**

*EPITECH 2025*

</div>
