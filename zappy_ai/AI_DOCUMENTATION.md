# 🤖 Zappy AI Documentation

Technical documentation for the Zappy AI client, written in Rust.

## 📋 Table of Contents

- [Overview](#-overview)
- [Architecture](#-architecture)
- [Dependencies](#-dependencies)
- [Installation & Build](#-installation--build)
- [Usage](#-usage)
- [Core Components](#-core-components)
- [Decision System](#-decision-system)
- [Commands](#-commands)
- [Resources & Inventory](#-resources--inventory)
- [Level Requirements](#-level-requirements)
- [Communication Protocol](#-communication-protocol)

---

## 🎯 Overview

The Zappy AI is an autonomous client that connects to the Zappy server and plays the game automatically. It uses a **decision tree** to determine the best actions based on the current game state, managing survival (food), resource collection, and team coordination for elevation rituals.

### Key Features

- **Asynchronous networking** with Tokio
- **Decision tree-based AI** for intelligent gameplay
- **Team coordination** via broadcast system
- **Automatic resource management**
- **Level-up coordination** with teammates

---

## 🏗 Architecture

```
zappy_ai/
├── Cargo.toml              # Project configuration & dependencies
├── Makefile                # Build automation
└── src/
    ├── main.rs             # Entry point, CLI argument parsing
    ├── client.rs           # ZappyClient: TCP connection & command execution
    ├── error.rs            # Custom error types
    ├── mod.rs              # Module declarations
    ├── map.rs              # Map representation (optional)
    ├── commands/
    │   ├── mod.rs          # Commands module
    │   ├── commands.rs     # Command enum & serialization
    │   └── broadcast.rs    # Broadcast system for team communication
    ├── decision/
    │   ├── mod.rs          # Decision module
    │   ├── decision_tree.rs    # Decision tree nodes (Food, Resource, LevelUp)
    │   └── decision_caller.rs  # Main decision loop & action handlers
    └── drone/
        ├── mod.rs          # Drone module
        ├── inventory.rs    # Inventory & Resource management
        ├── levels.rs       # Level requirements for elevation
        ├── player_state.rs # Player state tracking
        └── providers.rs    # Resource providers
```

---

## 📦 Dependencies

| Crate | Version | Purpose |
|-------|---------|---------|
| `tokio` | 1.0 | Async runtime (full features) |
| `clap` | 4.0 | CLI argument parsing |
| `serde` | 1.0 | Serialization/Deserialization |
| `serde_json` | 1.0 | JSON handling |
| `rand` | 0.9.1 | Random number generation |
| `async-trait` | 0.1.88 | Async trait support |

---

## 🔧 Installation & Build

### Prerequisites

- **Rust** (2021 edition or later)
- **Cargo** (Rust's package manager)

### Building

```bash
cd zappy_ai
make
```

Or using Cargo directly:

```bash
cargo build --release
```

The binary will be located at `target/debug/zappy_ai` or `target/release/zappy_ai`.

---

## 🚀 Usage

```bash
./zappy_ai -p <port> -n <team_name> [-m <machine>] [-d]
```

### Command Line Arguments

| Argument | Short | Long | Default | Description |
|----------|-------|------|---------|-------------|
| Port | `-p` | `--port` | `4242` | Server port number |
| Team Name | `-n` | `--name` | *required* | Team name to join |
| Machine | `-m` | `--machine` | `localhost` | Server hostname |
| Debug | `-d` | `--debug` | `false` | Enable debug output |

### Examples

```bash
# Basic connection
./zappy_ai -p 4242 -n team1

# Connect to remote server with debug
./zappy_ai -p 4242 -n team1 -m 192.168.1.100 -d

# Using long options
./zappy_ai --port 4242 --name team1 --machine localhost --debug
```

---

## 🧩 Core Components

### ZappyClient (`client.rs`)

The main client structure handling TCP communication with the server.

```rust
pub struct ZappyClient {
    reader: BufReader<OwnedReadHalf>,    // Async TCP reader
    writer: BufWriter<OwnedWriteHalf>,   // Async TCP writer
    pub team_name: String,               // Team identifier
    client_num: i32,                     // Available slots
    map_width: usize,                    // Map dimensions
    map_height: usize,
    freq: u32,                           // Server frequency
    pub debug: bool,                     // Debug mode flag
    pub last_look: Option<Vec<String>>,  // Cached look result
    pub player_state: PlayerState,       // Current player state
    broadcast: BroadcastSystem,          // Team communication
    pending_messages: VecDeque<String>,  // Message queue
    message_buffer: VecDeque<String>,    // Message buffer
    last_message_time: Instant,          // Timing
}
```

#### Connection Flow

```
1. Connect to server via TCP
2. Receive "WELCOME" message
3. Send team name
4. Receive:
   - Number of available slots (int)
   - Map dimensions (width height)
5. Initialize client structure
6. Start decision loop
```

#### Key Methods

| Method | Return Type | Description |
|--------|-------------|-------------|
| `connect()` | `Result<ZappyClient>` | Establish connection to server |
| `execute_command()` | `Result<String>` | Send command and get response |
| `forward()` | `Result<bool>` | Move forward one tile |
| `right()` / `left()` | `Result<bool>` | Rotate 90° |
| `look()` | `Result<Vec<String>>` | Get visible tiles |
| `inventory()` | `Result<Inventory>` | Get current inventory |
| `take(resource)` | `Result<bool>` | Pick up resource |
| `set(resource)` | `Result<bool>` | Drop resource |
| `broadcast(msg)` | `Result<bool>` | Send message to all players |
| `fork()` | `Result<bool>` | Lay an egg |
| `incantation()` | `Result<bool>` | Start elevation ritual |
| `connect_nbr()` | `Result<i32>` | Get available team slots |
| `eject()` | `Result<bool>` | Push players from tile |

---

## 🧠 Decision System

The AI uses a **decision tree** pattern to determine actions based on priorities.

### Priority Levels

```rust
pub enum Priority {
    Critical,  // Must act immediately
    High,      // Important action
    Low,       // Can wait
}
```

### Actions

```rust
pub enum Action {
    FindFood,         // Search for food
    CollectResource,  // Gather resources
    LevelUp,          // Perform incantation
    LayEgg,           // Fork new player
    JoinTeam,         // Coordinate with team
    MaintainFood,     // Maintain food supply
    Explore,          // Random exploration
    Wait,             // No action needed
}
```

### Decision Tree Nodes

```
┌─────────────────┐
│   FoodNode      │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  ResourceNode   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  LevelUpNode    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   JoinNode      │
└─────────────────┘
```

### Priority-Based Decision System

The AI decision system is **priority-driven**: each node in the tree returns both an **Action** and a **Priority level**. The priority determines which action gets executed when multiple actions are possible.

#### How Priority Works

When a node evaluates the game state, it returns a `(Priority, Action)` tuple. The main loop then uses the priority to decide:

| Priority | Behavior | Example Situations |
|----------|----------|-------------------|
| **Critical** | Executed immediately, overrides everything | Level-up ready, about to die |
| **High** | Executed as soon as possible | Low food, resource nearby |
| **Low** | Executed only when nothing urgent | Exploration, waiting |

### Decision Flow

| Node | Evaluation | Priority Returned | Action |
|------|------------|-------------------|--------|
| **FoodNode** | Food count < threshold? | `High` | `FindFood` |
| **FoodNode** | Food count >= threshold | *passes to next node* | — |
| **ResourceNode** | Has all resources for level 4+? | `Critical` | `LevelUp` |
| **ResourceNode** | Food > 8, needs resources | `High` | `CollectResource` |
| **LevelUpNode** | Can coordinate elevation? | `Critical` | `LevelUp` |
| **JoinNode** | Teammate requesting help? | `High` | `JoinTeam` |
| **Default** | Nothing else to do | `Low` | `Explore` / `Wait` |


This priority system ensures **survival-critical actions** and **team coordination** take precedence over individual resource gathering.

---

## ⚙️ Commands

### Command Enum

```rust
pub enum Command {
    Forward,              // Move forward
    Right,                // Turn right
    Left,                 // Turn left
    Look,                 // View surroundings
    Inventory,            // Check inventory
    Broadcast(String),    // Send message
    ConnectNbr,           // Check available slots
    Fork,                 // Lay egg
    Eject,                // Push players
    Take(Resource),       // Pick up item
    Set(Resource),        // Drop item
    Incantation,          // Start elevation
}
```

### Command Durations

Commands take time based on server frequency:

| Command | Base Time (ticks) |
|---------|-------------------|
| Forward | 7 |
| Right / Left | 7 |
| Look | 7 |
| Inventory | 1 |
| Broadcast | 7 |
| Take / Set | 7 |
| Eject | 7 |
| Fork | 42 |
| Incantation | 300 |
| Connect_nbr | 0 |

**Actual duration** = `base_time / frequency`

---

## 📦 Resources & Inventory

### Resource Types

```rust
pub enum Resource {
    Food,       // For survival
    Linemate,   // Stone
    Deraumere,  // Stone
    Sibur,      // Stone
    Mendiane,   // Stone
    Phiras,     // Stone
    Thystame,   // Stone (rarest)
}
```

### Inventory Structure

```rust
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

### Parsing Server Response

Inventory response format: `[food 10, linemate 2, deraumere 0, ...]`

```rust
impl Inventory {
    pub fn from_response(response: &str) -> Result<Self, ClientError> {
        // Parse the response string into Inventory struct
    }
}
```

---

## 📊 Level Requirements

To level up, players must perform an **incantation ritual** with:
- The correct number of players at the same level
- Specific resources on the current tile

### Elevation Table

| Level | Players Required | Resources Needed |
|-------|-----------------|------------------|
| 1 → 2 | 1 | 1 linemate |
| 2 → 3 | 2 | 1 linemate, 1 deraumere, 1 sibur |
| 3 → 4 | 2 | 2 linemate, 1 sibur, 2 phiras |
| 4 → 5 | 4 | 1 linemate, 1 deraumere, 2 sibur, 1 phiras |
| 5 → 6 | 4 | 1 linemate, 2 deraumere, 1 sibur, 3 mendiane |
| 6 → 7 | 6 | 1 linemate, 2 deraumere, 3 sibur, 1 phiras |
| 7 → 8 | 6 | 2 linemate, 2 deraumere, 2 sibur, 2 mendiane, 2 phiras, 1 thystame |

---

## 📡 Communication Protocol

### Server Messages

| Message | Description |
|---------|-------------|
| `WELCOME` | Connection established |
| `ok` | Command succeeded |
| `ko` | Command failed |
| `dead` | Player died (no food) |
| `message K, text` | Broadcast received (K = direction) |
| `Elevation underway` | Incantation started |
| `Current level: N` | Incantation succeeded |

### Broadcast Directions

When receiving a broadcast, the direction `K` indicates where it came from:

```
     1
   8   2
  7  0  3
   6   4
     5
```
- `0` = Same tile
- `1-8` = Direction relative to player orientation

### Team Coordination

The AI uses broadcasts to coordinate with teammates for:
- Level-up rituals
- Resource sharing
- Position information

---

## 🔄 Main Loop

```rust
pub async fn make_decision(client: &mut ZappyClient) -> Result<(), ClientError> {
    let decision_tree = DecisionTree::new();

    loop {
        // Evaluate decision tree
        let (priority, action) = decision_tree.evaluate(client).await;
        
        // Execute action based on priority
        match (priority, action) {
            (Priority::High, Action::FindFood) => handle_food_search(client).await?,
            (Priority::High, Action::CollectResource) => handle_resource_collection(client).await?,
            (Priority::Critical, Action::LevelUp) => handle_level_up(client).await?,
            (Priority::High, Action::LayEgg) => handle_lay_egg(client).await?,
            (Priority::High, Action::JoinTeam) => handle_join_team(client).await?,
            (Priority::Low, Action::Explore) => handle_exploration(client).await?,
            _ => sleep(Duration::from_secs(1)).await,
        }
        
        // Process any received broadcasts
        client.process_broadcasts().await?;
        client.reset_look_cache();
    }
}
```

---

## 🐛 Debug Mode

Enable debug mode with `-d` flag to see:
- All sent commands
- All received responses
- Connection details
- Decision tree evaluations

```bash
./zappy_ai -p 4242 -n team1 -d
```

Example output:
```
Connecting to localhost:4242
Received: WELCOME
Sent: team1
Received: 5
Received: 20 20
Connected successfully. Map size: 20x20
Connected to server going to make decision
Executing Look command
Sent: Look
Received: [ player food linemate, food, , sibur ]
Look result: ["player food linemate", "food", "", "sibur"]
```

---

## 📝 Error Handling

### Error Types

```rust
pub enum ClientError {
    IoError(std::io::Error),
    InvalidResponse(String),
    NoSlotsAvailable,
    ConnectionFailed,
    // ...
}
```

The AI gracefully handles:
- Connection failures
- Invalid server responses
- Network timeouts
- Full teams (no slots available)

---

*Part of the Zappy Project — EPITECH 2025*
