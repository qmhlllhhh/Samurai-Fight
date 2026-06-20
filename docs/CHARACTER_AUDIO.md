# 角色特定音效与场景背景音乐

本文档说明如何使用角色特定音效和场景背景音乐功能。

## 1. 角色特定音效

### 1.1 配置角色音效

在角色配置文件中添加 `sounds` 字段：

```json
{
    "id": "musashi",
    "name": "武藏",
    "sounds": {
        "hurt": "musashi_hurt",
        "death": "musashi_death",
        "attack_light": "musashi_attack_light",
        "attack_medium": "musashi_attack_medium",
        "attack_heavy": "musashi_attack_heavy",
        "block_success": "musashi_block",
        "jump": "musashi_jump",
        "land": "musashi_land"
    }
}
```

每个音效类型映射到 ResourceManager 中的音效 ID。

### 1.2 加载角色音效配置

在角色初始化时加载音效配置：

```cpp
// 在 Character::loadConfig() 中
bool Character::loadConfig(const std::string& configPath) {
    // ... 其他配置加载 ...
    
    // 加载角色音效配置
    AudioManager::getInstance().loadCharacterSounds(m_id, configPath);
    
    return true;
}
```

### 1.3 播放角色特定音效

```cpp
// 受伤时
AudioManager::getInstance().playCharacterSound("musashi", "hurt");

// 死亡时
AudioManager::getInstance().playCharacterSound("musashi", "death");

// 攻击时
AudioManager::getInstance().playCharacterSound("musashi", "attack_light");
AudioManager::getInstance().playCharacterSound("musashi", "attack_medium");
AudioManager::getInstance().playCharacterSound("musashi", "attack_heavy");

// 格挡成功
AudioManager::getInstance().playCharacterSound("musashi", "block_success");

// 跳跃和落地
AudioManager::getInstance().playCharacterSound("musashi", "jump");
AudioManager::getInstance().playCharacterSound("musashi", "land");
```

### 1.4 在 Character 类中集成

推荐在 Character 类中添加辅助方法：

```cpp
// Character.h
class Character {
public:
    void playSound(const std::string& soundType, float volumeScale = 1.0f);
    
private:
    std::string m_id;  // 角色ID
};

// Character.cpp
void Character::playSound(const std::string& soundType, float volumeScale) {
    AudioManager::getInstance().playCharacterSound(m_id, soundType, volumeScale);
}

// 使用
void Character::takeDamage(int damage) {
    m_health -= damage;
    playSound("hurt");  // 自动播放该角色的受伤音效
    
    if (m_health <= 0) {
        playSound("death");
    }
}
```

## 2. 场景背景音乐

### 2.1 预定义场景类型

AudioManager 内置了以下场景音乐映射：

| 场景类型           | 音乐文件                       |
| ------------------ | ------------------------------ |
| `main_menu`        | assets/music/menu_theme.ogg    |
| `character_select` | assets/music/select_theme.ogg  |
| `battle`           | assets/music/battle_theme.ogg  |
| `victory`          | assets/music/victory_theme.ogg |
| `result`           | assets/music/result_theme.ogg  |

### 2.2 在场景中播放背景音乐

```cpp
// MainMenuScene.cpp
void MainMenuScene::enter() {
    // 进入菜单场景时播放菜单音乐（淡入2秒）
    AudioManager::getInstance().playSceneMusic("main_menu", 2.0f);
}

void MainMenuScene::exit() {
    // 退出时停止音乐（淡出1秒）
    AudioManager::getInstance().stopMusic(1.0f);
}

// BattleScene.cpp
void BattleScene::enter() {
    // 进入战斗场景时播放战斗音乐
    AudioManager::getInstance().playSceneMusic("battle", 1.5f);
}

// ResultScene.cpp
void ResultScene::enter() {
    // 根据结果播放不同音乐
    if (m_winner == Player1) {
        AudioManager::getInstance().playSceneMusic("victory", 1.0f);
    } else {
        AudioManager::getInstance().playSceneMusic("result", 1.0f);
    }
}
```

### 2.3 场景切换示例

```cpp
// 从主菜单进入角色选择
void MainMenuScene::onStartButtonClicked() {
    // 淡出当前音乐
    AudioManager::getInstance().stopMusic(1.0f);
    
    // 切换场景
    SceneManager::getInstance().changeScene("character_select");
}

// CharacterSelectScene 会自动播放选择界面音乐
void CharacterSelectScene::enter() {
    AudioManager::getInstance().playSceneMusic("character_select", 1.0f);
}
```

## 3. 音效文件组织

### 3.1 目录结构

```
assets/sounds/
├── combat/              # 通用战斗音效
│   ├── hit_light.wav
│   ├── hit_medium.wav
│   └── hit_heavy.wav
├── movement/            # 通用移动音效
│   ├── jump.wav
│   └── land.wav
├── ui/                  # UI音效
│   ├── menu_select.wav
│   └── menu_confirm.wav
├── round/               # 回合音效
│   ├── round_start.wav
│   └── victory.wav
└── characters/          # 角色特定音效
    ├── musashi/
    │   ├── hurt.wav
    │   ├── death.wav
    │   ├── attack_light.wav
    │   ├── attack_medium.wav
    │   ├── attack_heavy.wav
    │   ├── block.wav
    │   ├── jump.wav
    │   └── land.wav
    └── sakura/
        ├── hurt.wav
        ├── death.wav
        ├── attack_light.wav
        ├── attack_medium.wav
        ├── attack_heavy.wav
        ├── block.wav
        ├── jump.wav
        └── land.wav

assets/music/            # 背景音乐
├── menu_theme.ogg
├── select_theme.ogg
├── battle_theme.ogg
├── victory_theme.ogg
└── result_theme.ogg
```

### 3.2 音效格式建议

- **音效**: WAV 格式（短音效，快速加载）
- **背景音乐**: OGG 格式（压缩率高，流式播放）

## 4. 完整示例：战斗场景集成

```cpp
// BattleScene.cpp
#include "../managers/AudioManager.h"

void BattleScene::enter() {
    // 播放战斗背景音乐
    AudioManager::getInstance().playSceneMusic("battle", 2.0f);
}

void BattleScene::update(float deltaTime) {
    // AudioManager 会在 Game::update() 中自动更新
    // 无需在此调用
}

void BattleScene::onCharacterHit(Character& attacker, Character& defender, const std::string& attackType) {
    // 播放攻击者的攻击音效
    attacker.playSound("attack_" + attackType);
    
    // 播放防御者的受伤音效
    defender.playSound("hurt");
}

void BattleScene::onCharacterBlock(Character& blocker) {
    // 播放格挡成功音效
    blocker.playSound("block_success");
}

void BattleScene::onRoundEnd(const std::string& winnerId) {
    // 播放回合结束音效
    AudioManager::getInstance().playSound("round_end");
    
    // 切换到胜利音乐
    AudioManager::getInstance().playSceneMusic("victory", 1.0f);
}

void BattleScene::exit() {
    // 淡出战斗音乐
    AudioManager::getInstance().stopMusic(1.5f);
}
```

## 5. 音量控制

```cpp
// 设置主音量（影响所有音频）
AudioManager::getInstance().setMasterVolume(0.8f);  // 80%

// 单独设置背景音乐音量
AudioManager::getInstance().setBGMVolume(0.6f);     // 60%

// 单独设置音效音量
AudioManager::getInstance().setSFXVolume(1.0f);     // 100%

// 实际音量 = masterVolume × specificVolume
// 例如：BGM实际音量 = 0.8 × 0.6 = 0.48 (48%)
```

## 6. 注意事项

1. **音效预加载**: 所有角色音效在 ResourceManager 初始化时预加载
2. **角色配置加载**: 需要在角色初始化时调用 `loadCharacterSounds()`
3. **音乐淡入淡出**: 建议场景切换时使用淡入淡出，提升体验
4. **音效命名规范**: `{characterId}_{soundType}` 格式，如 `musashi_hurt`
5. **性能**: 角色音效不会重复加载，ResourceManager 会缓存所有音效
