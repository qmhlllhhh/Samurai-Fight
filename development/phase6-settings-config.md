# 阶段6：设置与配置

> 状态：✅ 已完成（音量 + 键位重映射 + AudioManager 实时通路，均已合并对接）
> 完成日期：2026年6月21日
> 对应路线图：阶段 6（第 8 周）
> 交付物：完整的设置系统

---

## 目录

- [一、概述](#一概述)
- [二、目标与交付物](#二目标与交付物)
- [三、关键现状审计（不冲突依据）](#三关键现状审计不冲突依据)
- [四、总体设计](#四总体设计)
- [五、新增/改造类设计](#五新增改造类设计)
- [六、对现有文件的修改点](#六对现有文件的修改点)
- [七、数据配置](#七数据配置)
- [八、新增库与头文件](#八新增库与头文件)
- [九、CMakeLists 改动](#九cmakelists-改动)
- [十、实现里程碑](#十实现里程碑)
- [十一、风险与注意事项](#十一风险与注意事项)

---

## 一、概述

阶段 6 实现游戏的设置系统：**设置界面、键位重映射、音量控制、配置文件保存/加载**。

### 设计原则

1. **复用优先**：ConfigManager 的音量 getter 已存在，补 setter + save；`JsonLoader::saveToFile` 已存在，直接用。
2. **音量双通路**：ConfigManager 负责持久化（写回 JSON），AudioManager 负责实时（立即影响播放）。两者命名一致（`setBGMVolume/setSFXVolume` 大写）。
3. **严格对齐 guide 项目结构**：`AudioManager→managers/`、`SettingsScene→scenes/`（guide 无 `audio/` 目录，故 AudioManager 放 managers）。
4. **不重复造轮子**：实现时发现 InputManager 已是事实上的"键位管理器"，故**扩展 InputManager**（而非新建 KeyBindingManager），保持 `getPlayerInput` 对外接口不变。

---

## 二、目标与交付物

对应 [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) §七 阶段 6：

| 路线图条目 | 落地为 | 状态 |
| ---------- | ------ | ---- |
| 设置界面 | `src/scenes/SettingsScene.h/.cpp` | ✅ |
| 键位重映射 | 扩展 InputManager（`keyToString`/`scancodeToString`）+ ConfigManager `setKeyBinding` | ✅ |
| 音量控制 | ConfigManager setter + AudioManager 实时通路（已对接组员 AudioManager） | ✅ |
| 配置文件保存/加载 | ConfigManager `saveGameConfig`/`saveKeyBindings` | ✅ |

### 完成内容

- **音量**：值存储/读取/保存（ConfigManager）+ 实时生效（AudioManager，调音量时 BGM/音效立即变化）
- **键位**：加载/重映射/保存（向导式，scancode 捕获，支持字母/数字/小键盘/方向/修饰键）
- **设置界面**：SettingsScene（键盘 + 鼠标，悬停/点击/滚轮）
- **入口**：主菜单 "Settings" + 暂停菜单 "Settings"

---

## 三、关键现状审计（不冲突依据）

| 现状 | 位置 | 阶段 6 复用/补全 |
| ---- | ---- | ---------------- |
| 音量 getter 已有 | [ConfigManager](../src/managers/ConfigManager.cpp) | **复用** |
| 音量 setter / save 缺失 | ConfigManager | **补** `setMasterVolume` 等 + `saveGameConfig`/`saveKeyBindings` |
| `JsonLoader::saveToFile` 已有 | [JsonLoader](../src/utils/JsonLoader.cpp) | **复用** |
| InputManager 已有键位逻辑（map、`loadKeyBindingsFromJson`、`stringToKey`、默认键位） | [InputManager](../src/input/InputManager.cpp) | **扩展**（加 `keyToString`/`scancodeToString`），不新建类 |
| `SoundBuffer` 加载已有 | [ResourceManager](../src/managers/ResourceManager.cpp) | **复用** |
| 无 SettingsScene | — | **新建** `scenes/SettingsScene` |
| AudioManager（组员实现） | [managers/AudioManager](../src/managers/AudioManager.cpp) | **对接**（音量实时通路） |

---

## 四、总体设计

### 4.1 对象关系

```
Game
 ├── ConfigManager          ← 配置"真相来源"（音量/键位/窗口，getter + setter + save）
 │
 ├── AudioManager（组员）   ← 单例，读 ConfigManager 音量，负责实际播放
 │
 └── SceneManager
      └── SettingsScene     ← 设置界面（音量/键位/返回）
           ├── 读写 ConfigManager（音量 + 键位）
           ├── 实时调 AudioManager（音量立即生效）
           └── 返回时 save 持久化（game_config.json + keybindings.json）
```

### 4.2 音量数据流（双通路，均已打通）

```
SettingsScene 调音量
   ├─→ ConfigManager.setMasterVolume(v)         ✅ 持久化（写 m_gameConfig["audio"]）
   └─→ AudioManager::getInstance().setXXX(v)    ✅ 实时（BGM/音效立即变化）
退出时：ConfigManager.saveGameConfig()           写回 game_config.json
启动时：AudioManager::initialize()                读初始音量（组员已接入 Game::initialize）
```

---

## 五、新增/改造类设计

### 5.1 ConfigManager 改造（补 setter + save + setKeyBinding）

**文件**：[ConfigManager.h](../src/managers/ConfigManager.h) / [.cpp](../src/managers/ConfigManager.cpp) ✅

```cpp
void setMasterVolume(float v);   // 音量 setter（0-1，自动钳制）
void setBGMVolume(float v);
void setSFXVolume(float v);
bool saveGameConfig(const std::string& path);     // 写回 game_config.json
bool saveKeyBindings(const std::string& path);    // 写回 keybindings.json
void setKeyBinding(int player, const std::string& action, const std::string& keyName);  // 改键位
```

### 5.2 AudioManager（组员实现，已对接）

组员的 AudioManager（[managers/AudioManager](../src/managers/AudioManager.h)）已合并。接口：**单例** `AudioManager::getInstance()`，`setMasterVolume/setBGMVolume/setSFXVolume`（大写，与 ConfigManager 一致）+ getter + `playSound/playMusic` + `initialize()`。

**对接**（已在 [SettingsScene::applyVolume](../src/scenes/SettingsScene.cpp) 启用实时调用）：

```cpp
void SettingsScene::applyVolume(SettingsItem item, float v) {
    ConfigManager& cfg = Game::getInstance().getConfigManager();
    cfg.setMasterVolume(v);                         // ✅ 持久化
    AudioManager::getInstance().setMasterVolume(v); // ✅ 实时（BGM/音效立即变化）
}
```

> AudioManager 的初始化（`Game::initialize` 调 `AudioManager::getInstance().initialize()`）、每帧 `update`、`shutdown` 清理均由组员在 [Game.cpp](../src/core/Game.cpp) 接入，本阶段无需改 Game。

### 5.3 键位重映射（扩展 InputManager）

> 实现时发现 InputManager 已是事实上的"键位管理器"（已有 action→Key 映射、`loadKeyBindingsFromJson`、`stringToKey`、默认键位）。故**不新建 KeyBindingManager**（避免重复），而是**扩展 InputManager + ConfigManager**。

**改动**：

| 文件 | 新增 |
| ---- | ---- |
| [InputManager](../src/input/InputManager.cpp) | `keyToString(Key)`（static）、`scancodeToString(Scancode)`（static，**重映射捕获用**）；`stringToKey` 扩展主键盘数字 Num0-9 |
| [ConfigManager](../src/managers/ConfigManager.cpp) | `setKeyBinding(player, action, keyName)` |
| [SettingsScene](../src/scenes/SettingsScene.cpp) | 键位重映射向导：捕获用 `k->scancode`，`Tab` 前进、`Esc` 取消 |

**重映射交互**（向导式，SettingsScene 的 "Rebind Keys" 项）：

- 选 "Rebind Keys" → 逐个动作提示（P1 10 个 + P2 10 个 = 20 步）
- 按键 → `scancodeToString` → `setKeyBinding`（**不前进**，看屏幕 `(current: ...)` 确认生效）
- `Tab` → 前进到下一个动作（无论改没改）
- `Esc` → 取消

**关键：用 scancode 而非 code**

SFML 3.0 的 `KeyPressed.code`（虚拟键码）在某些键盘布局下对字母返回 -1、数字 Unknown；而 `KeyPressed.scancode`（物理扫描码）总是有效。故**捕获用 scancode**。存储/查询仍用 Key（`stringToKey` → `isKeyPressed(Key)`，战斗中已验证工作）。

完整链路：`scancode → scancodeToString → "A" → setKeyBinding → keybindings.json → stringToKey("A") → Key::A → isKeyPressed`。

### 5.4 SettingsScene（scenes/）

**文件**：[SettingsScene.h](../src/scenes/SettingsScene.h) / [.cpp](../src/scenes/SettingsScene.cpp) ✅

参照 [MainMenuScene](../src/scenes/MainMenuScene.cpp) 的键盘+鼠标交互模式。菜单项：

1. **Master/BGM/SFX Volume** —— `Left/Right`、鼠标滚轮、或**鼠标点击左半减/右半加**（整数百分比步进，避免浮点误差）
2. **Rebind Keys** —— 进入键位重映射向导（见 §5.3）
3. **Back (Save)** —— 保存（音量 + 键位）并返回上层

支持鼠标：移动悬停选中、左键确认/调值、滚轮调音量。

---

## 六、对现有文件的修改点

| 文件 | 修改 | 冲突风险 |
| ---- | ---- | -------- |
| [ConfigManager.h/.cpp](../src/managers/ConfigManager.cpp) | 补 setter + save + `setKeyBinding` | 纯新增方法 |
| [InputManager.h/.cpp](../src/input/InputManager.cpp) | 加 `keyToString`/`scancodeToString`（static）、`stringToKey` 扩展 | 纯新增，`getPlayerInput` 不变 |
| [MainMenuScene](../src/scenes/MainMenuScene.cpp) | "设置"菜单项 → push SettingsScene | 加入口 |
| [PauseScene](../src/scenes/PauseScene.cpp) | "设置"菜单项 → push SettingsScene | 加入口 |
| [CMakeLists.txt](../CMakeLists.txt) | 加 SettingsScene.cpp | 见第九节 |

> AudioManager 的接入（Game::initialize/update/shutdown）由组员完成，本阶段只对接音量实时通路。

---

## 七、数据配置

- `assets/config/game_config.json` 的 `audio` 字段已存在，save 写回。
- `assets/config/keybindings.json` 已存在，save 写回。
- ⚠️ **保存路径见风险 1**（构建目录 assets vs 源 assets）。

---

## 八、新增库与头文件

### 第三方库

**无需新增**。SFML Audio 已链接。

### 新增头文件

| 新文件 | include |
| ------ | ------- |
| `scenes/SettingsScene.h` | `Scene.h`、`<SFML/Graphics.hpp>` |
| `managers/AudioManager.h`（组员） | `<SFML/Audio.hpp>` 等 |

---

## 九、CMakeLists 改动

```cmake
    # Scenes (Phase 6)
    src/scenes/SettingsScene.cpp
```

> AudioManager.cpp 由组员在 CMakeLists 添加；KeyBindingManager 未新建（扩展 InputManager 代替）。

---

## 十、实现里程碑

1. ✅ **ConfigManager setter + save + setKeyBinding**（配置持久化）
2. ✅ **AudioManager 实时通路**（已合并组员 AudioManager，SettingsScene 调音量实时生效）
3. ✅ **键位重映射**（扩展 InputManager + scancode 捕获）
4. ✅ **SettingsScene**（音量 + 键位 UI，键盘+鼠标）
5. ✅ **集成入口**（主菜单 + 暂停 → 设置）

---

## 十一、风险与注意事项

1. **保存路径**：exe 在 `build/bin/`，加载/保存的是 `build/bin/assets/config/*.json`（构建时从源 assets 复制）。重启 exe 保留；重新 `cmake --build` 会被源 assets 覆盖。后续应 save 到源 assets 或用户配置目录。
2. **键位重映射用 scancode**：SFML 3.0 事件 `code` 对字母/数字可能 -1/Unknown，必须用 `scancode` 捕获（见 §5.3）。
3. **InputManager 扩展保持接口**：`getPlayerInput` 返回的 `InputState` 不变，不波及 BattleScene/Character。
4. **SettingsScene 返回语义**：从 MainMenu 进 → 返回 MainMenu；从 Pause 进 → 返回 Pause（`popScene`）。

---

## 附录：阶段5 UI 系统（Timer / ScoreDisplay / ResultScene）✅ 已实现

> 阶段5 剩余 UI（已实现）。对应 [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) §七 阶段5（Timer、ScoreDisplay、结果界面）与 §四 `ui/` 组件规划。追加于本文档。

### A.1 目标

| guide 要求 | 落地为 |
| ---------- | ------ |
| 对战界面计时器（§2.5.1 第3项） | `src/ui/Timer.h/.cpp` |
| 比分显示 | `src/ui/ScoreDisplay.h/.cpp` |
| 结果界面（§2.5.1 第6项） | `src/scenes/ResultScene.h/.cpp` |

### A.2 现状审计（不冲突依据）

| 现状 | 位置 | 阶段5 复用 |
| ---- | ---- | ---------- |
| HealthBar/StaminaBar UI 组件模式 | [ui/](../src/ui/) | **参照**（position/size + update/render + sf::Text/Shape） |
| MatchHud 已含计时+比分+过渡 | [MatchHud](../src/battle/MatchHud.cpp)（phase4） | **改用** Timer+ScoreDisplay 组件（过渡文字 MatchHud 自留） |
| MainMenuScene/PauseScene 菜单模式 | scenes | **参照**（ResultScene 键盘+鼠标） |
| MatchManager 数据 | [MatchManager](../src/battle/MatchManager.cpp) | getMatchWinner / getRoundsWon（ResultScene、ScoreDisplay） |
| RoundManager 数据 | [RoundManager](../src/battle/RoundManager.cpp) | getTimeRemaining（Timer） |
| BattleScene onMatchEnd 钩子 | [BattleScene](../src/scenes/BattleScene.cpp) | MatchOver → 切 ResultScene |

### A.3 设计

#### A.3.1 Timer（ui/）

**文件**：[ui/Timer.h](../src/ui/Timer.h) / [Timer.cpp](../src/ui/Timer.cpp) ✅
**职责**：显示回合计时秒数，剩余 ≤10 秒变红警告。

```cpp
class Timer {
public:
    Timer(const sf::Vector2f& position, unsigned int characterSize = 48);
    void update(int secondsRemaining);   // 显示秒数（<=10 红色警告）
    void render(sf::RenderWindow& window);
    void setPosition(const sf::Vector2f& position);
};
```

数据源：`RoundManager::getTimeRemaining()`。

#### A.3.2 ScoreDisplay（ui/）

**文件**：[ui/ScoreDisplay.h](../src/ui/ScoreDisplay.h) / [ScoreDisplay.cpp](../src/ui/ScoreDisplay.cpp) ✅
**职责**：显示比分（如 `1 - 0`）+ 胜场指示（已赢得的局用实心圆点）。

```cpp
class ScoreDisplay {
public:
    ScoreDisplay(const sf::Vector2f& position, int winsNeeded);
    void update(int roundsWonP1, int roundsWonP2);  // "X - Y" + 胜场圆点
    void render(sf::RenderWindow& window);
};
```

数据源：`MatchManager::getRoundsWon(0/1)`。

#### A.3.3 ResultScene（scenes/）

**文件**：[scenes/ResultScene.h](../src/scenes/ResultScene.h) / [ResultScene.cpp](../src/scenes/ResultScene.cpp) ✅
**职责**：比赛结果界面（guide §2.5.1 第6项）。

- **显示**：胜者（`Player X Wins!`）+ 比分（`2 - 1`）
- **菜单**：Rematch（重赛，同角色新 BattleScene）/ Character Select（返回选人）/ Main Menu（返回主菜单）
- **交互**：参照 MainMenuScene（键盘 Up/Down/Enter + 鼠标悬停/点击）

```cpp
class ResultScene : public Scene {
public:
    ResultScene(int winner, int roundsWonP1, int roundsWonP2,
                const std::string& p1CharId, const std::string& p2CharId);
    // onEnter/handleEvents/update/render
};
```

**触发**：`BattleScene::onMatchEnd` MatchOver 时：
```cpp
m_nextScene = std::make_unique<ResultScene>(winner, roundsWonP1, roundsWonP2,
                                            m_player1CharacterId, m_player2CharacterId);
m_popSceneCount = 1;  // 替换 BattleScene
```

### A.4 对现有文件修改

| 文件 | 修改 | 冲突风险 |
| ---- | ---- | -------- |
| [MatchHud](../src/battle/MatchHud.cpp) | 内部计时/比分改用 Timer+ScoreDisplay 组件（过渡文字保留） | 内部重构，对外接口不变 |
| [BattleScene::onMatchEnd](../src/scenes/BattleScene.cpp) | MatchOver 切 ResultScene（替换 phase4 的"直接 changeState(Victory)"） | 改钩子逻辑 |
| [CMakeLists.txt](../CMakeLists.txt) | 加 Timer.cpp / ScoreDisplay.cpp / ResultScene.cpp | 加源文件 |

### A.5 实现里程碑（全部完成）

1. ✅ **Timer** 组件（计时显示 + ≤10 秒红色警告）
2. ✅ **ScoreDisplay** 组件（比分 + 胜场圆点）
3. ✅ **MatchHud 改用** Timer+ScoreDisplay
4. ✅ **ResultScene**（胜者/比分/菜单）
5. ✅ **BattleScene 切 ResultScene** + 联调（重赛/返回角色选择/主菜单）

---

**文档版本**：1.3
**最后更新**：2026年6月21日
**对应阶段**：Phase 6 - 设置与配置（附录含阶段5 UI，均已实现）
**状态**：✅ 已完成
