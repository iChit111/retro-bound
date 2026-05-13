# Pac-Man — Modern C++ with SFML

A full Pac-Man implementation showcasing **modern C++ game development**,
built as a contrast to the Turbo C++ / `conio.h` era.

---

## What's Different From Turbo C++?

| Feature              | Turbo C++ (your project)         | This project (SFML)                      |
|----------------------|----------------------------------|------------------------------------------|
| Rendering            | `gotoxy` + ASCII characters      | Hardware-accelerated 2D shapes & sprites |
| Movement             | Instant tile-hop on keypress     | Smooth pixel movement with delta time    |
| Animation            | Character swapping (`^v<>`)      | Pie-chart mouth open/close each frame    |
| Ghost AI             | None / simple random             | Blinky/Pinky/Inky/Clyde personalities   |
| Game states          | One `do-while` loop              | Full state machine (menu/play/die/win)   |
| Build system         | Single `.cpp` in TC++ IDE        | CMake — works in VSCode, CLion, etc.     |
| Memory management    | Raw `new` only                   | RAII with smart pointers possible        |
| Standards            | Pre-standard C++ (C++98 ish)     | C++17                                    |

---

## Prerequisites

Install SFML for your OS:

```bash
# Ubuntu / Debian
sudo apt install libsfml-dev cmake build-essential

# Arch Linux
sudo pacman -S sfml cmake

# macOS (Homebrew)
brew install sfml cmake

# Windows — use vcpkg
vcpkg install sfml
```

---

## Build & Run

```bash
# 1. Clone / copy this folder, then:
cd pacman-sfml

# 2. Configure with CMake
cmake -B build

# 3. Build
cmake --build build

# 4. Run
./build/pacman          # Linux/macOS
build\Debug\pacman.exe  # Windows
```

### VSCode Setup

1. Install extensions: **C/C++** and **CMake Tools**
2. Open the `pacman-sfml/` folder in VSCode
3. Press `Ctrl+Shift+P` → **CMake: Configure**
4. Press `F5` to build and run

---

## Controls

| Key              | Action          |
|------------------|-----------------|
| W / ↑            | Move Up         |
| S / ↓            | Move Down       |
| A / ←            | Move Left       |
| D / →            | Move Right      |
| Enter            | Start / Restart |

---

## Ghost Personalities

| Ghost  | Colour | Behaviour                                         |
|--------|--------|---------------------------------------------------|
| Blinky | Red    | Always targets your exact tile — relentless       |
| Pinky  | Pink   | Targets 4 tiles **ahead** of you — ambushes       |
| Inky   | Cyan   | Flanks using Blinky's position as a pivot         |
| Clyde  | Orange | Chases when far away, retreats when close — shy!  |

---

## Project Structure

```
pacman-sfml/
├── main.cpp          ← Entry point (tiny on purpose)
├── Game.h / .cpp     ← Game loop, state machine, HUD, scoring
├── Maze.h / .cpp     ← Tile grid, dot eating, wall queries
├── Pacman.h / .cpp   ← Player movement, mouth animation, death
├── Ghost.h / .cpp    ← All 4 ghosts: movement + AI
└── CMakeLists.txt    ← Cross-platform build config
```

---

## Key Concepts Demonstrated

- **Delta-time movement** — `pos += speed * dt` so the game runs the same speed
  on any hardware (vs. Turbo C++'s `delay()` calls)
- **State machine** — clean separation of Menu / Playing / Dying / Win states
- **Polymorphism** — `Ghost` could be a base class; each personality overrides
  `getChaseTarget()` (shown as a switch here for readability)
- **Separation of concerns** — Maze doesn't know about ghosts; ghosts don't draw the HUD
- **RAII** — SFML handles GPU resource cleanup when objects go out of scope
