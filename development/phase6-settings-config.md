# 阶段6：设置与配置

> 状态：🔧 进行中（音量 + 键位重映射已完成；AudioManager 实时通路等组员合并）
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
2. **音量先做接口**：音量值存储 + 读取接口先行，实际音效播放等资源到位后集成（AudioManager 预留）。
3. **严格对齐 guide 项目结构**：`AudioManager→managers/`、`SettingsScene→scenes/`（guide 无 `audio/` 目录，故 AudioManager 放 managers）。
4. **不重复造轮子**：实现时发现 InputManager 已是事实上的"键位管理器"，故**扩展 InputManager**（而非新建 KeyBindingManager），保持 `getPlayerInput` 对外接口不变。

---

## 二、目标与交付物

对应 [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) §七 阶段 6：

| 路线图条目 | 落地为 | 状态 |
| ---------- | ------ | ---- |
| 设置界面 | `src/scenes/SettingsScene.h/.cpp` | ✅ |
| 键位重映射 | 扩展 InputManager（`keyToString`/`scancodeToString`）+ ConfigManager `setKeyBinding` | ✅ |
| 音量控制 | ConfigManager setter（+ AudioManager 实时通路，等组员） | ✅ 持久化 / ⏳ 实时 |
| 配置文件保存/加载 | ConfigManager `saveGameConfig`/`saveKeyBindings` | ✅ |

### 边界说明

- **已完成**：音量值存储/读取/保存、键位加载/重映射/保存、设置界面 UI（键盘+鼠标）、配置写回 JSON。
- **延后（等组员 AudioManager 合并）**：音量实时通路（`AudioManager::setXXXVolume`）、实际音效播放。

---

## 三、关键现状审计（不冲突依据）

| 现状 | 位置 | 阶段 6 复用/补全 |
| ---- | ---- | ---------------- |
| 音量 getter 已有 | [ConfigManager](../src/managers/ConfigManager.cpp) | **复用** |
| 音量 setter / save 缺失 | ConfigManager | **补** `setMasterVolume` 等 + `saveGameConfig`/`saveKeyBindings` |
| `JsonLoader::saveToFile` 已有 | [JsonLoader](../src/utils/JsonLoader.cpp) | **复用** |
| InputManager 已有键位逻辑（map、`loadKeyBindingsFromJson`、`stringToKey`、默认键位） | [InputManager](../src/input/InputManager.cpp) | **扩展**（加 `keyToString`/`scancodeToString`），不新建类 |
| `SoundBuffer` 加载已有 | [ResourceManager](../src/managers/ResourceManager.cpp) | **复用**（音效来后） |
| 无 SettingsScene | — | **新建** `scenes/SettingsScene` |
| 无播放管理 | — | **组员实现** `managers/AudioManager` |

---

## 四、总体设计

### 4.1 对象关系

```
Game
 ├── ConfigManager          ← 配置"真相来源"（音量/键位/窗口，getter + setter + save）
 │
 ├── AudioManager（组员）   ← 预留：读 ConfigManager 音量，播放（stub，等资源）
 │
 └── SceneManager
      └── SettingsScene     ← 设置界面（音量/键位/返回）
           ├── 读写 ConfigManager（音量 + 键位）
           └── 返回时 save 持久化（game_config.json + keybindings.json）
```

### 4.2 音量数据流（双通路）

```
SettingsScene 调音量
   ├─→ ConfigManager.setMasterVolume(v)         ✅ 持久化（已实现）
   └─→ AudioManager::getInstance().setXXX(v)    ⏳ 实时（组员合并后启用）
退出时：ConfigManager.saveGameConfig()           写回 JSON
启动时：AudioManager::getInstance().loadConfig()  读初始值
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

### 5.2 AudioManager（组员实现中，音量引擎）

> 由组员负责。组员已发使用文档 `AUDIO_USAGE.md`，接口定稿为：**单例** `AudioManager::getInstance()`，`setMasterVolume/setBGMVolume/setSFXVolume`（大写，与 ConfigManager 一致）+ `getMasterVolume` + `playSound/playMusic` 等。文件 `src/managers/AudioManager.h`（与推荐位置一致）。

**对接骨架**（组员合并后取消 SettingsScene 注释即可）：

```cpp
void SettingsScene::applyVolume(SettingsItem item, float v) {
    ConfigManager& cfg = Game::getInstance().getConfigManager();
    cfg.setMasterVolume(v);                         // ✅ 已实现
    // AudioManager::getInstance().setMasterVolume(v); // ⏳ 组员合并后启用
}
// 启动：AudioManager::getInstance().loadConfig("assets/config/game_config.json");
```

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

1. **Master/BGM/SFX Volume** —— `Left/Right` 或鼠标滚轮调（整数百分比步进，避免浮点误差）
2. **Rebind Keys** —— 进入键位重映射向导（见 §5.3）
3. **Back (Save)** —— 保存（音量 + 键位）并返回上层

支持鼠标：移动悬停选中、左键确认、滚轮调音量。

---

## 六、对现有文件的修改点

| 文件 | 修改 | 冲突风险 |
| ---- | ---- | -------- |
| [ConfigManager.h/.cpp](../src/managers/ConfigManager.cpp) | 补 setter + save + `setKeyBinding` | 纯新增方法 |
| [InputManager.h/.cpp](../src/input/InputManager.cpp) | 加 `keyToString`/`scancodeToString`（static）、`stringToKey` 扩展 | 纯新增，`getPlayerInput` 不变 |
| [MainMenuScene](../src/scenes/MainMenuScene.cpp) | "设置"菜单项 → push SettingsScene | 加入口 |
| [Game.cpp](../src/core/Game.cpp) | （待）initialize 创建 AudioManager + loadConfig | 待组员 |
| [CMakeLists.txt](../CMakeLists.txt) | 加 SettingsScene.cpp | 见第九节 |

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

> AudioManager.cpp / KeyBindingManager.cpp 不在本阶段 CMake（AudioManager 待组员合并；KeyBindingManager 未新建）。

---

## 十、实现里程碑

1. ✅ **ConfigManager setter + save + setKeyBinding**（配置持久化）
2. ⏳ **AudioManager 实时通路**（等组员合并）
3. ✅ **键位重映射**（扩展 InputManager + scancode 捕获）
4. ✅ **SettingsScene**（音量 + 键位 UI，键盘+鼠标）
5. ✅ **集成入口**（主菜单 → 设置）；PauseScene 入口待加

---

## 十一、风险与注意事项

1. **保存路径**：exe 在 `build/bin/`，加载/保存的是 `build/bin/assets/config/*.json`（构建时从源 assets 复制）。重启 exe 保留；重新 `cmake --build` 会被源 assets 覆盖。后续应 save 到源 assets 或用户配置目录。
2. **音量 stub 不能崩**：AudioManager 未合并前 SettingsScene 的实时调用为注释。
3. **键位重映射用 scancode**：SFML 3.0 事件 `code` 对字母/数字可能 -1/Unknown，必须用 `scancode` 捕获（见 §5.3）。
4. **InputManager 扩展保持接口**：`getPlayerInput` 返回的 `InputState` 不变，不波及 BattleScene/Character。
5. **SettingsScene 返回语义**：从 MainMenu 进 → 返回 MainMenu；从 Pause 进 → 返回 Pause（`popScene`）。

---

**文档版本**：1.1
**最后更新**：2026年6月20日
**对应阶段**：Phase 6 - 设置与配置
**状态**：🔧 进行中（音量+键位已完成，AudioManager 等组员）
