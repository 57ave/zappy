# 🧠 ZAPPY PROJECT

Welcome to the Zappy project, a multiplayer network game where teams compete in a 2D world to achieve supreme elevation.

## 📋 Table of Contents

- [About](#-about)
- [Project Goal](#-project-goal)
- [The World of Trantor](#-the-world-of-trantor)
- [Project Structure](#-project-structure)
- [Getting Started](#-getting-started)
- [Documentation](#-documentation)
- [Team](#-team)

## 🎯 About

Zappy is a multiplayer network game inspired by science fiction. Players are immersed in a 2D world named **Trantor**, where multiple teams compete to achieve supreme elevation — a symbol of power and wisdom in this peaceful world.

## 🏆 Project Goal

Each player embodies a **Trantorian**, a curious and non-violent being, evolving on a map containing various resources. The objective is to help your team progress: **the first team to have at least 6 members reach level 8 wins the game**.

## 🌍 The World of Trantor

The world is a 2D flat map that is **toroidal** — exit from one side and you'll reappear on the opposite side.

Natural resources spawn over time:
- 🍗 **Food** — essential for survival
- 💎 **Minerals** — `linemate`, `deraumere`, `sibur`, `mendiane`, `phiras`, `thystame`

These resources are fundamental for survival and ascending through levels via the **incantation ritual**.

## 📁 Project Structure

The project consists of three main components:

| Component | Language | Description |
|-----------|----------|-------------|
| **Server** (`zappy_server`) | C | Game server managing connections, map, resources, and game logic |
| **AI Client** (`zappy_ai`) | Rust | Autonomous AI player that connects to the server and plays the game |
| **GUI Client** (`zappy_gui`) | C++ / SFML | Graphical interface to visualize the game in real-time |

## 🚀 Getting Started

### Prerequisites

- **GCC** (for the server)
- **Rust & Cargo** (for the AI)
- **SFML** (for the GUI)

### Building the Project

Build all components from the root directory:

```bash
make
```

Or build each component individually:

```bash
# Server
cd server && make

# AI Client
cd zappy_ai && make

# GUI Client
cd zappy_gui && make
```

### Running the Game

#### 1. Start the Server

```bash
./zappy_server -p <port> -x <width> -y <height> -n <team1> <team2> ... -c <clientsNb> -f <freq>
```

**Options:**
| Flag | Description |
|------|-------------|
| `-p` | Port number |
| `-x` | Map width |
| `-y` | Map height |
| `-n` | Team names (space-separated) |
| `-c` | Number of clients allowed per team |
| `-f` | Frequency (time unit divider) |

**Example:**
```bash
./zappy_server -p 4242 -x 20 -y 20 -n team1 team2 -c 5 -f 100
```

#### 2. Start the AI Client(s)

```bash
./zappy_ai/target/debug/zappy_ai -p <port> -n <team_name> -m <machine>
```

**Options:**
| Flag | Description |
|------|-------------|
| `-p` | Server port (default: 4242) |
| `-n` | Team name to join |
| `-m` | Server hostname (default: localhost) |
| `-d` | Enable debug mode |

**Example:**
```bash
./zappy_ai/target/debug/zappy_ai -p 4242 -n team1 -m localhost
```

#### 3. Start the GUI

```bash
./zappy_gui -p <port> -h <machine>
```

**Options:**
| Flag | Description |
|------|-------------|
| `-p` | Server port |
| `-h` | Server hostname |

**Example:**
```bash
./zappy_gui -p 4242 -h localhost
```

### GUI Controls

| Key | Action |
|-----|--------|
| `Z` | Move view up |
| `Q` | Move view left |
| `S` | Move view down |
| `D` | Move view right |
| `+` | Zoom in |
| `-` | Zoom out |
| `Enter` | Switch from menu to game view |

## 📚 Documentation

- 📖 [User Documentation](documentation/USER_DOCUMENTATION_ZAPPY.txt) — How to play the game
- 🔧 [Developer Documentation](documentation/DEV_DOCUMENTATION_ZAPPY.txt) — Technical details for developers
- 🤖 [AI Technical Documentation](zappy_ai/AI_DOCUMENTATION.md) — Detailed Rust AI implementation guide

## 👥 Team

**Project by:** Aymen | Erwan | Gustave | Zoltan
