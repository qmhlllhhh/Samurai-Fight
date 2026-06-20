# 音频系统使用示例

## 基础使用

### 1. 在场景中播放背景音乐

```cpp
#include "../managers/AudioManager.h"

// 在场景进入时播放背景音乐（淡入2秒）
void MainMenuScene::onEnter() {
    AudioManager::getInstance().playMusic("assets/music/menu_theme.ogg", true, 2.0f);
}

// 在场景退出时停止音乐（淡出1秒）
void MainMenuScene::onExit() {
    AudioManager::getInstance().stopMusic(1.0f);
}
```

### 2. 播放音效

```cpp
// 基础音效播放
AudioManager::getInstance().playSound("menu_select");

// 带音量缩放的音效（例如：远处的攻击声）
AudioManager::getInstance().playSound("hit_light", 0.5f);
```

### 3. 音量控制

```cpp
// 设置主音量（影响所有音频）
AudioManager::getInstance().setMasterVolume(0.8f);

// 设置背景音乐音量
AudioManager::getInstance().setBGMVolume(0.6f);

// 设置音效音量
AudioManager::getInstance().setSFXVolume(1.0f);

// 获取当前音量
float masterVol = AudioManager::getInstance().getMasterVolume();
```

## 在战斗场景中的使用示例

### 角色攻击音效

在 `Character.cpp` 或攻击状态中：

```cpp
void Character::performAttack(AttackType type) {
    switch (type) {
        case AttackType::Light:
            AudioManager::getInstance().playSound("hit_light");
            break;
        case AttackType::Medium:
            AudioManager::getInstance().playSound("hit_medium");
            break;
        case AttackType::Heavy:
            AudioManager::getInstance().playSound("hit_heavy");
            break;
    }
}
```

### 角色移动音效

```cpp
void Character::jump() {
    AudioManager::getInstance().playSound("jump");
    // ... 跳跃逻辑
}

void Character::land() {
    AudioManager::getInstance().playSound("land");
    // ... 着地逻辑
}
```

### 格挡音效

```cpp
void Character::onBlockSuccess() {
    AudioManager::getInstance().playSound("block_success");
}

void Character::onBlockBreak() {
    AudioManager::getInstance().playSound("block_break");
}
```

## 在UI中的使用示例

### 菜单导航

```cpp
void MainMenuScene::onMenuSelectionChange() {
    AudioManager::getInstance().playSound("menu_select");
}

void MainMenuScene::onMenuConfirm() {
    AudioManager::getInstance().playSound("menu_confirm");
}
```

### 回合系统

```cpp
void RoundManager::onRoundStart() {
    AudioManager::getInstance().playSound("round_start");
}

void RoundManager::onRoundEnd() {
    AudioManager::getInstance().playSound("round_end");
}

void RoundManager::onVictory() {
    AudioManager::getInstance().playSound("victory");
}
```

## 音乐切换示例

```cpp
// 从主菜单进入战斗场景
void BattleScene::onEnter() {
    // 停止菜单音乐（自动淡出）
    // 播放战斗音乐（淡入1.5秒）
    AudioManager::getInstance().playMusic("assets/music/battle_theme.ogg", true, 1.5f);
}

// 暂停游戏时
void PauseScene::onEnter() {
    AudioManager::getInstance().pauseMusic();
}

void PauseScene::onExit() {
    AudioManager::getInstance().resumeMusic();
}
```

## 注意事项

1. **音效预加载**：所有音效在游戏启动时由 `ResourceManager` 预加载，无需担心运行时加载延迟。

2. **多音效同时播放**：系统支持最多 32 个音效同时播放，超过时会自动停止最早的已播放音效。

3. **背景音乐流式播放**：背景音乐使用流式播放，不会占用大量内存，适合较长的音乐文件。

4. **淡入淡出**：背景音乐切换时会自动处理淡入淡出，避免突兀的音量变化。

5. **音量计算公式**：
   - 音效最终音量 = `masterVolume × sfxVolume × volumeScale × 100`
   - 背景音乐最终音量 = `masterVolume × bgmVolume × 100`

6. **配置文件**：音量设置从 `assets/config/game_config.json` 读取：
   ```json
   {
       "audio": {
           "masterVolume": 1.0,
           "bgmVolume": 0.8,
           "sfxVolume": 1.0
       }
   }
   ```

## 完整集成示例

### BattleScene.cpp 完整示例

```cpp
#include "BattleScene.h"
#include "../managers/AudioManager.h"

void BattleScene::onEnter() {
    // 播放战斗背景音乐
    AudioManager::getInstance().playMusic("assets/music/battle_theme.ogg", true, 1.5f);
}

void BattleScene::onExit() {
    // 停止战斗音乐
    AudioManager::getInstance().stopMusic(1.0f);
}

void BattleScene::handleAttack(Character& attacker, Character& defender) {
    // 播放攻击音效
    AudioManager::getInstance().playSound("hit_medium");

    // ... 攻击处理逻辑
}

void BattleScene::handleBlock(Character& blocker) {
    // 播放格挡音效
    AudioManager::getInstance().playSound("block_success");

    // ... 格挡处理逻辑
}
```
