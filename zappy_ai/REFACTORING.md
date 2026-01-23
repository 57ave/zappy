# 🔄 Zappy AI Refactoring Guide

> **A deep dive into the architectural improvements made to the Rust AI client**

*January 2026*

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Problems Identified](#-problems-identified)
- [Solutions Implemented](#-solutions-implemented)
- [Before vs After](#-before-vs-after)
- [Key Design Decisions](#-key-design-decisions)
- [Lessons Learned](#-lessons-learned)

---

## 🎯 Overview

This document explains the **major refactoring** performed on the Zappy AI client to bring it up to **production-grade backend engineering standards**. The refactoring focused on:

1. **Eliminating race conditions** in TCP communication
2. **Type-safe protocol parsing** replacing string comparisons
3. **DRY (Don't Repeat Yourself)** command execution
4. **Robust error handling** without panics

### Goals

| Goal | Status |
|------|--------|
| No race conditions on TCP reader | ✅ Achieved |
| Type-safe server event handling | ✅ Achieved |
| Single point of network I/O | ✅ Achieved |
| No `unwrap()` in production paths | ✅ Achieved |
| Idiomatic Rust patterns | ✅ Achieved |
| Recruiter-friendly code quality | ✅ Achieved |

---

## 🐛 Problems Identified

### 1. Race Condition on TCP Reader

**Problem:** The original code had multiple methods directly reading from the TCP stream:

```rust
// ❌ BEFORE: Multiple methods reading from TCP
async fn look(&mut self) -> Result<Vec<String>, ClientError> {
    self.writer.write_all(b"Look\n").await?;
    let response = self.reader.read_line(&mut buf).await?; // Direct read!
    // ...
}

async fn inventory(&mut self) -> Result<Inventory, ClientError> {
    self.writer.write_all(b"Inventory\n").await?;
    let response = self.reader.read_line(&mut buf).await?; // Another direct read!
    // ...
}
```

**Issue:** When commands overlap (e.g., a broadcast arrives while waiting for `Look` response), the wrong method receives the wrong data, causing:
- Response mismatches
- Parse errors
- Unpredictable behavior

### 2. Stringly-Typed Protocol Parsing

**Problem:** Server responses were handled with raw string comparisons:

```rust
// ❌ BEFORE: String comparisons everywhere
if response == "ok" {
    // success
} else if response == "ko" {
    // failure
} else if response.starts_with("[") {
    // could be Look or Inventory... how to tell?
}
```

**Issues:**
- Easy to misspell ("OK" vs "ok")
- No compile-time safety
- Difficult to distinguish Look from Inventory (both start with `[`)
- Scattered parsing logic

### 3. Code Duplication

**Problem:** Similar patterns repeated across command methods:

```rust
// ❌ BEFORE: Repeated patterns
async fn forward(&mut self) -> Result<bool, ClientError> {
    self.writer.write_all(b"Forward\n").await?;
    self.writer.flush().await?;
    let response = self.read_response().await?;
    if response == "ok" { Ok(true) } else { Ok(false) }
}

async fn left(&mut self) -> Result<bool, ClientError> {
    self.writer.write_all(b"Left\n").await?;
    self.writer.flush().await?;
    let response = self.read_response().await?;
    if response == "ok" { Ok(true) } else { Ok(false) }
}
// ... same pattern for right, take, set, etc.
```

### 4. No Graceful Error Handling for Edge Cases

**Problem:** The code didn't handle protocol edge cases:

```rust
// ❌ BEFORE: No handling for delayed responses
// Server log shows: command queued, ok/ko sent later
// Client receives stale ok/ko when expecting Look response
```

---

## ✅ Solutions Implemented

### 1. Single TCP Reader Pattern

**Solution:** All TCP reads now go through ONE method: `read_response_filtered()`

```rust
// ✅ AFTER: Single point of TCP reading
async fn read_response_filtered(&mut self) -> Result<ServerEvent, ClientError> {
    loop {
        let response = self.read_response().await?;
        let event = self.parse_event(&response)?;
        
        match event {
            // Broadcast messages are queued, not returned
            ServerEvent::Message(_, _) => {
                self.pending_messages.push_back(response);
                continue;
            },
            // Death is an error, not an event
            ServerEvent::Dead => {
                return Err(ClientError::ConnectionError("Player is dead".to_string()));
            },
            // All other events are returned to caller
            _ => return Ok(event),
        }
    }
}
```

**Benefits:**
- ✅ No race conditions — only one reader
- ✅ Broadcasts automatically queued
- ✅ Death handled immediately
- ✅ Clear data flow

### 2. Type-Safe ServerEvent Enum

**Solution:** Created a comprehensive enum for all server responses:

```rust
// ✅ AFTER: Type-safe event handling
pub enum ServerEvent {
    Ok,
    Ko,
    Dead,
    ElevationUnderway,
    Message(String, String),      // (direction, content)
    Inventory(Inventory),         // Parsed inventory struct
    Value(i32),                   // Numeric response
    Look(Vec<String>),            // Tile contents
    Unknown(String),              // Fallback
}
```

**Parser with Classification System:**

```rust
impl FromStr for ServerEvent {
    type Err = ParseError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let input = s.trim();
        
        // Dispatch based on message type
        match Self::classify(input) {
            EventType::Simple    => Self::parse_simple(input),
            EventType::Elevation => Ok(Self::ElevationUnderway),
            EventType::Message   => Self::parse_message(input),
            EventType::Bracketed => Self::parse_bracketed(input),
            EventType::Numeric   => Self::parse_numeric(input),
            EventType::Unknown   => Ok(Self::Unknown(input.to_string())),
        }
    }
}
```

**Look vs Inventory Disambiguation:**

```rust
// Both are bracketed [...], but have different formats:
// Inventory: [food 10, linemate 2, ...]     <- all "name count" pairs
// Look:      [player food, linemate, ...]   <- mixed content

fn parse_bracketed(input: &str) -> Result<Self, ParseError> {
    let segments: Vec<&str> = content.split(',').map(|s| s.trim()).collect();
    
    // Check if ALL segments match inventory pattern
    let is_inventory = segments.iter().all(|segment| {
        let words: Vec<&str> = segment.split_whitespace().collect();
        words.len() == 2 
            && words[0] != "player" 
            && words[1].parse::<i32>().is_ok()
    });
    
    if is_inventory {
        Inventory::from_response(input).map(Self::Inventory)
    } else {
        Ok(Self::Look(segments.iter().map(|t| t.to_string()).collect()))
    }
}
```

### 3. Centralized Command Execution

**Solution:** Single `execute_command()` method for all commands:

```rust
// ✅ AFTER: Centralized command execution
pub async fn execute_command(&mut self, command: Command) -> Result<ServerEvent, ClientError> {
    let cmd_str = format!("{}\n", command.to_string());
    if self.debug {
        println!("Sent: {}", cmd_str.trim());
    }
    self.writer.write_all(cmd_str.as_bytes()).await?;
    self.writer.flush().await?;
    self.read_response_filtered().await
}

// Helper for ok/ko responses
fn send_and_expect_ok(&self, response: ServerEvent) -> Result<bool, ClientError> {
    match response {
        ServerEvent::Ok => Ok(true),
        ServerEvent::Ko => Ok(false),
        _ => Err(ClientError::invalid_response("expected ok or ko"))
    }
}

// Commands are now one-liners
pub async fn forward(&mut self) -> Result<bool, ClientError> {
    let response = self.execute_command(Command::Forward).await?;
    self.send_and_expect_ok(response)
}
```

### 4. Graceful Handling of Stale Responses

**Solution:** Commands that expect specific response types now filter stale ok/ko:

```rust
// ✅ AFTER: Filter stale responses
pub async fn look(&mut self) -> Result<Vec<String>, ClientError> {
    self.writer.write_all(b"Look\n").await?;
    self.writer.flush().await?;
    
    loop {
        let event = self.read_response_filtered().await?;
        match event {
            ServerEvent::Look(tiles) => return Ok(tiles),
            // Ignore stale ok/ko from previous commands
            ServerEvent::Ok | ServerEvent::Ko => {
                if self.debug {
                    println!("Ignoring stale ok/ko response");
                }
                continue;
            },
            _ => return Err(ClientError::invalid_response("Expected Look response")),
        }
    }
}
```

---

## 📊 Before vs After

### Code Quality Comparison

| Metric | Before | After |
|--------|--------|-------|
| TCP read points | Multiple (5+) | **1** |
| Response type safety | Strings | **Enum** |
| Race conditions | Possible | **Eliminated** |
| `unwrap()` calls | Many | **0 in prod paths** |
| Code duplication | High | **Minimal (DRY)** |
| Error handling | Inconsistent | **Comprehensive** |

### Example: Handling a Look Command

**Before:**
```rust
// Scattered, unsafe, race-prone
async fn look(&mut self) -> Result<Vec<String>, ClientError> {
    self.writer.write_all(b"Look\n").await?;
    let mut response = String::new();
    self.reader.read_line(&mut response).await?;  // Direct read!
    
    if response.starts_with("[") {
        // Parse manually... hope it's not an inventory response
        let tiles = response.trim_matches(|c| c == '[' || c == ']')
            .split(',')
            .map(|s| s.to_string())
            .collect();
        Ok(tiles)
    } else {
        Err(ClientError::invalid_response("Expected look response"))
    }
}
```

**After:**
```rust
// Centralized, type-safe, robust
pub async fn look(&mut self) -> Result<Vec<String>, ClientError> {
    self.writer.write_all(b"Look\n").await?;
    self.writer.flush().await?;
    
    loop {
        let event = self.read_response_filtered().await?;
        match event {
            ServerEvent::Look(tiles) => return Ok(tiles),
            ServerEvent::Ok | ServerEvent::Ko => continue,  // Filter stale
            _ => return Err(ClientError::invalid_response("Expected Look")),
        }
    }
}
```

---

## 🏛️ Key Design Decisions

### Why a Single TCP Reader?

In async Rust, multiple tasks awaiting on the same reader causes **unpredictable behavior**. The first task to wake up gets the data, regardless of which command it was meant for.

**Decision:** Funnel ALL reads through `read_response_filtered()`, which:
1. Reads one line
2. Parses into `ServerEvent`
3. Handles special cases (broadcasts, death)
4. Returns to caller

### Why an Enum Over Strings?

**Type safety:** The compiler ensures you handle all cases:

```rust
match event {
    ServerEvent::Ok => { /* handled */ },
    ServerEvent::Ko => { /* handled */ },
    ServerEvent::Look(tiles) => { /* handled */ },
    // Compiler error if you forget a variant!
}
```

**No typos:** You can't write `ServerEvent::Okk` — it won't compile.

### Why Filter Stale ok/ko?

The server processes commands asynchronously. When you send `Take food`, it may be queued. Later, when you send `Look`, the server might respond:
1. `ok` (from Take food, finally processed)
2. `[player food, ...]` (from Look)

Without filtering, the `look()` method would receive `ok` and error out.

---

## 📚 Lessons Learned

### 1. Async Network Clients Need Discipline

In synchronous code, you can read directly after writing. In async code, **anything can happen between await points**. Design for that.

### 2. Parse Early, Use Types Everywhere

Don't pass strings around. Parse into domain types (enums, structs) at the boundary, then use those types throughout your codebase.

### 3. Single Responsibility for I/O

One reader. One writer. One parser. Makes debugging trivial.

### 4. Test Against Real Protocol Logs

The best way to validate protocol handling is to capture real server logs and verify your client handles every edge case.

---

## 🔗 Related Files

| File | Purpose |
|------|---------|
| `src/client.rs` | Main client with refactored methods |
| `src/server_event.rs` | `ServerEvent` enum and parsing |
| `src/error.rs` | Comprehensive error types |
| `src/commands/commands.rs` | Command enum |

---

## ✅ Checklist for Future Changes

When modifying the client, ensure:

- [ ] All TCP reads go through `read_response_filtered()`
- [ ] New response types are added to `ServerEvent`
- [ ] Commands use `execute_command()` or the filtering pattern
- [ ] No `unwrap()` on user data
- [ ] Tests cover edge cases (stale responses, broadcasts)

---

<div align="center">

**Refactoring by Gustave Delecroix**

*January 2026*

</div>
