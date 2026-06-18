# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Samurai Fight (武士对决) is a 2D pixel-art 1v1 fighting game built with C++ and SFML 3.0.

- **Language**: C++17/20
- **Graphics**: SFML 3.0
- **Build System**: CMake 3.16+
- **JSON Parser**: nlohmann/json
- **Target**: 60 FPS, 1280×720 resolution

## Build Commands

```bash
# Configure the project
cmake -B build

# Build
cmake --build build

# Run the game
./build/SamuraiFight
```

## Architecture

### Core Systems

```
Game (Singleton)
├── ResourceManager    - Loads and caches textures, fonts, sounds
├── SceneManager       - Manages scene stack and transitions
├── ConfigManager      - Handles JSON configuration files
└── FrameTimer         - Enforces 60 FPS fixed timestep
```

### Scene Hierarchy

```
Scene (base)
├── MainMenuScene
├── CharacterSelectScene
├── BattleScene
│   ├── MatchManager → RoundManager
│   ├── Character[2] with Components
│   └── BattleUI
├── PauseScene (overlay)
├── SettingsScene
└── ResultScene
```

### Character Components

Each Character has:
- **StateMachine**: Manages character states (Idle, Move, Jump, Crouch, Attack, Block, Hurt, Victory)
- **InputBuffer**: 12-frame input history for combo detection
- **HealthComponent**: HP tracking and damage
- **StaminaComponent**: Stamina for movement/blocking
- **AnimationComponent**: Sprite animation playback
- **HitboxComponent**: Attack and hurt boxes

### Frame-Locked Game Loop

The game uses a fixed timestep loop at 60 FPS:

```cpp
void Game::run() {
    while (window->isOpen()) {
        frameTimer.update();
        
        while (frameTimer.shouldUpdate()) {
            handleEvents();
            update(FRAME_TIME);  // Fixed 1/60 second
            frameTimer.decreaseAccumulator();
        }
        
        render();
    }
}
```

## Key Game Systems

### Input Buffer (Combo System)

- 12-frame buffer window for combo inputs
- Light/Medium attacks can cancel during defined cancel windows
- Heavy attacks cannot be canceled
- Input commands are collected for both players before frame update

### Stamina System

- Moving: consumes 0.1 stamina per frame
- Jumping: consumes 10 stamina
- Blocking: consumes (damage × 0.5) when hit
- Standing: recovers 0.5 stamina per frame
- Exhausted: movement slowed 50%, blocking fails

### Combat Frame Data

| Attack  | Startup | Active | Recovery | Can Cancel |
|---------|---------|--------|----------|------------|
| Light   | 3       | 2      | 5        | Yes (2-5)  |
| Medium  | 5       | 3      | 8        | Yes (4-8)  |
| Heavy   | 8       | 4      | 12       | No         |

### Round System

- 60 seconds per round
- Best of 3 (first to 2 wins)
- Winner determined by: HP > 0 or more HP when time expires

## Code Conventions

### Naming

- **Classes**: PascalCase (`Character`, `GameState`)
- **Functions**: camelCase (`updatePosition`, `takeDamage`)
- **Variables**: camelCase (`currentHealth`, `moveSpeed`)
- **Constants**: UPPER_SNAKE_CASE (`MAX_HEALTH`, `TARGET_FPS`)
- **Member variables**: `m_` prefix (`m_currentHealth`)
- **Static variables**: `s_` prefix (`s_instance`)

### File Organization

- One class per `.h`/`.cpp` pair
- Use `#pragma once` for header guards
- Implementation files include their corresponding header first

### Documentation

Use Doxygen-style comments:

```cpp
/**
 * @brief Brief description
 * 
 * Detailed description
 * 
 * @param paramName Parameter description
 * @return Return value description
 */
```

## Git Commit Format

```
<type>(<scope>): <subject>

<body>

<footer>
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

Example:
```
feat(combat): add combo system

- Implement input buffer for combo detection
- Add combo data loading from JSON
- Support 3-hit combo for musashi
```

## Configuration Files

All game data is loaded from JSON files in `assets/config/`:

- `game_config.json` - Window, gameplay, audio settings
- `keybindings.json` - Player 1 and 2 key mappings
- `characters/<name>/config.json` - Character stats, attacks, combos, animations, hitboxes

## SFML 3.0 Migration Notes

SFML 3.0 has breaking changes from 2.x:

```cpp
// Event handling
while (const std::optional event = window.pollEvent()) { ... }

// Texture loading
auto texture = sf::Texture::loadFromFile("path").value();
```

## Default Key Bindings

**Player 1**: WASD (move), J/K/L (light/medium/heavy), U (special), I (block)
**Player 2**: Arrows (move), Numpad 1-5 (attacks/block)
**Global**: Escape (pause)

## Performance Targets

- Frame rate: Stable 60 FPS
- Memory: < 500 MB
- Startup: < 3 seconds
- Scene transitions: < 1 second
