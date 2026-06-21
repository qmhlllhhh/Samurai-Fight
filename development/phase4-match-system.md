# 阶段4：回合与比赛

> 状态：✅ 已完成
> 完成日期：2026年6月20日
> 对应路线图：阶段 4（第 6 周）
> 交付物：完整的回合制对战（三局两胜）

---

## 目录

- [一、概述](#一概述)
- [二、目标与交付物](#二目标与交付物)
- [三、关键现状审计（不冲突依据）](#三关键现状审计不冲突依据)
- [四、总体设计](#四总体设计)
- [五、新增类设计](#五新增类设计)
- [六、对现有文件的修改点](#六对现有文件的修改点)
- [七、数据配置](#七数据配置)
- [八、新增库与头文件](#八新增库与头文件)
- [九、CMakeLists 改动](#九cmakelists-改动)
- [十、实现里程碑](#十实现里程碑)
- [十一、风险与注意事项](#十一风险与注意事项)
- [十二、实测与已知限制](#十二实测与已知限制)
- [十三、快速导航](#十三快速导航)

---

## 一、概述

阶段 4 在阶段 3（战斗系统）的基础上，引入**回合制**与**比赛**流程，使对战从"单局无限打"升级为"三局两胜"的完整对局。

本阶段不重写任何现有战斗逻辑，而是**在 BattleScene 之上叠加一层回合/比赛调度器**（MatchManager → RoundManager），复用已有的角色、状态机、伤害、血量等系统。

### 设计原则

1. **不拥有角色**：MatchManager / RoundManager 只持有 `Character*` 引用，不改变角色生命周期。
2. **最小侵入**：对 BattleScene 的修改为"包裹式"（在原有输入/碰撞/渲染流程外加调度），不改原有逻辑。
3. **帧精确**：回合阶段切换与倒计时统一用固定步长 `FRAME_TIME` 驱动，保证 60 FPS 一致性。
4. **边界清晰**：正式结果界面（ResultScene）属阶段 5，本阶段仅做胜利展示并预留接入钩子。

---

## 二、目标与交付物

对应 [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) §7 阶段 4：

| 路线图条目 | 落地为 | 状态 |
| ---------- | ------ | ---- |
| RoundManager | `src/battle/RoundManager.h/.cpp` | ✅ |
| MatchManager | `src/battle/MatchManager.h/.cpp` | ✅ |
| 回合过渡动画 | RoundManager 内部 `RoundPhase` 状态机 + MatchHud 文字 | ✅ |
| 胜负判定 | RoundManager（单回合）+ MatchManager（整局计分） | ✅ |
| VictoryState | `src/states/VictoryState.h/.cpp` | ✅ |

### 边界说明

- **包含**：回合状态机、倒计时、HP 归零/时间到判定、比分局数、回合间角色重置、胜者 VictoryState、过渡文字（ROUND / FIGHT / K.O. / TIME UP / DRAW）、比赛胜负横幅。
- **不包含（留给阶段 5）**：正式 `ResultScene`、`ScoreDisplay`/`Timer` 的独立 UI 组件化。本阶段用 `MatchHud` 承担显示（已正式化，阶段 5 可平滑拆分）。

---

## 三、关键现状审计（不冲突依据）

下列能力已在代码中存在，阶段 4 直接复用，是"不冲突"的基础：

| 现状 | 位置 | 阶段 4 复用方式 |
| ---- | ---- | --------------- |
| `CharacterStateType::Victory` 枚举已存在（曾未实现） | [CharacterStateType.h](../src/states/CharacterStateType.h) | 实现 VictoryState 并在 `createState` 加一个 case |
| `CharacterStateType::Dead` + `DeadState` 已实现 | [DeadState.h](../src/states/DeadState.h) | VictoryState 照搬其结构 |
| 角色死亡自动触发：HP 归零时 `takeDamage()` 切到 Dead | [Character.cpp](../src/entities/Character.cpp) | RoundManager 每帧检测 `isDead()` 即可，无需改伤害逻辑 |
| `HealthComponent::reset()` 已存在 | [HealthComponent.h](../src/components/HealthComponent.h) | 回合间重置血量 |
| `StaminaComponent::reset()` 已存在（本阶段补充 `m_wasJumping` 重置） | [StaminaComponent.cpp](../src/components/StaminaComponent.cpp) | 回合间回满体力 |
| `Character::isDead()` / `getHealthComponent()` 已存在 | [Character.h](../src/entities/Character.h) | 回合判定用 |
| `ConfigManager::getRoundTime()` / `getWinsNeeded()` 已存在 | [ConfigManager.h](../src/managers/ConfigManager.h) | 回合时长、获胜局数直接读 |
| `Constants::ROUND_TIME/WINS_NEEDED/FRAME_TIME/GROUND_LEVEL` 已存在 | [Constants.h](../src/core/Constants.h) | 计时器、起点用 |
| 场景切换：`m_popSceneCount` + `m_nextScene`（先 pop N 个，再 push next） | [SceneManager.cpp](../src/managers/SceneManager.cpp) | 比赛结束切场景复用同一机制 |
| `ResourceManager::getDefaultFont()` 已在 BattleScene 使用 | [BattleScene.cpp](../src/scenes/BattleScene.cpp) | 过渡/比分/计时无需新字体 |

**唯一需要新增的能力**：`Character` 没有"回合间整体重置"方法 → 新增 `resetForRound()`。

---

## 四、总体设计

### 4.1 对象关系

```
BattleScene（拥有并驱动）
 ├── MatchManager            ← 管理"三局两胜"全局流程
 │    ├── m_roundsWon[2]     ← 双方胜场
 │    ├── m_roundNumber      ← 当前第几回合
 │    ├── m_matchState       ← InProgress / MatchOver
 │    └── RoundManager       ← 当前回合（unique_ptr）
 │
 └── RoundManager            ← 管理单个回合
      ├── m_phase            ← Intro → Ready → Fight → RoundOver
      ├── m_timeRemaining    ← 倒计时（秒）
      ├── m_result           ← 本回合结果（含 m_timeUp 区分 K.O./TIME UP）
      └── Character[2]*      ← 不拥有，引用 BattleScene 的角色

MatchHud                      ← 显示：过渡文字 / 倒计时 / 比分 / 胜负横幅
```

**关键原则**：MatchManager / RoundManager **不拥有 Character**，只持有指针引用 BattleScene 的 `m_characters[2]`。角色生命周期不变，BattleScene 原有输入/碰撞/渲染流程完全不动，只是被"包"进回合流程。

### 4.2 回合状态机（RoundPhase）

```
        ┌──────┐  ~60帧   ┌───────┐  ~30帧   ┌────────┐
  start │Intro │────────►│ Ready │────────►│ Fight  │
        └──────┘ "ROUND n"└───────┘"FIGHT!" └────────┘
                                            HP=0 或 time=0
                                                 │
                                                 ▼
                                            ┌──────────┐
                                            │RoundOver │ "K.O."/"TIME UP"
                                            └──────────┘
                                  短暂停留 │
                          ┌─────────────────┴────────────────┐
                 下一回合  │                       比赛结束   │
                          ▼                                  ▼
                   重置角色 → Ready              MatchOver → VictoryState / 横幅
```

每个 phase 用固定时长驱动（`FRAME_TIME` 累加）：Intro 1.0s、Ready 0.5s、RoundOver 停留 2.0s。

### 4.3 数据流

```
BattleScene::update(FRAME_TIME)
    │
    ├── 1. 输入收集（仅在 Fight 阶段传给角色，否则传全 false）
    ├── 2. 角色更新 / 碰撞检测（原有逻辑，保留）
    ├── 3. m_matchManager->update(FRAME_TIME)
    │        ├── RoundManager 推进 phase / 倒计时
    │        ├── checkRoundEnd()：HP=0 或 time=0 → 判定 → RoundOver
    │        └── 回合 isFinished → 计分 → 未达 winsNeeded 则自动 startNextRound
    ├── 4. 若 MatchOver → onMatchEnd()：切 ResultScene（阶段5 增强）
    └── 5. m_matchHud->update(...)：刷新过渡文字/比分/横幅
```

---

## 五、新增类设计

### 5.1 RoundManager（核心）

**文件**：[RoundManager.h](../src/battle/RoundManager.h) / [RoundManager.cpp](../src/battle/RoundManager.cpp)
**职责**：驱动单回合的生命周期与胜负判定。

```cpp
enum class RoundPhase { Intro, Ready, Fight, RoundOver };
enum class RoundResult { None, Player1Win, Player2Win, Draw };

class RoundManager {
public:
    RoundManager(Character* p1, Character* p2, int roundTime, int roundNumber);
    void start();                   ///< 进入 Intro（重置角色）
    void update(float deltaTime);   ///< 每帧驱动

    bool isInputLocked() const;     ///< Fight 阶段外锁定输入
    bool isFinished() const;        ///< RoundOver 停留期满
    RoundResult getResult() const;
    bool isTimeUp() const;          ///< 是否因时间到结束（区分 K.O./TIME UP）
    int getTimeRemaining() const;   ///< 显示用（向上取整秒）
    RoundPhase getPhase() const;
    int getRoundNumber() const;
    // ...
};
```

**胜负判定逻辑**（`checkRoundEnd`）：

| 触发条件 | 结果 |
| -------- | ---- |
| `m_players[0]->isDead()` | `Player2Win` |
| `m_players[1]->isDead()` | `Player1Win` |
| 同帧双杀（罕见） | `Draw` |
| `m_timeRemaining <= 0` 且 HP1 > HP2 | `Player1Win`（`m_timeUp=true`） |
| `m_timeRemaining <= 0` 且 HP2 > HP1 | `Player2Win`（`m_timeUp=true`） |
| `m_timeRemaining <= 0` 且 HP 相等 | `Draw`（`m_timeUp=true`） |

结果只判定一次，之后进入 `RoundOver` 并停留 2 秒。`isTimeUp()` 供 MatchHud 区分显示 `K.O.` 还是 `TIME UP`。

### 5.2 MatchManager

**文件**：[MatchManager.h](../src/battle/MatchManager.h) / [MatchManager.cpp](../src/battle/MatchManager.cpp)
**职责**：管理整局（三局两胜），调度回合切换与最终胜负。

```cpp
enum class MatchState { InProgress, MatchOver };

class MatchManager {
public:
    MatchManager(Character* p1, Character* p2, int roundTime, int winsNeeded);
    void update(float deltaTime);           ///< 驱动当前回合 + 处理回合结束
    void startNextRound();                  ///< 创建新 RoundManager 并 start
    RoundManager* getCurrentRound() const;
    int getRoundsWon(int playerIndex) const;
    int getRoundNumber() const;
    MatchState getMatchState() const;
    int getMatchWinner() const;             ///< -1=未决，0/1=胜者
    // ...
};
```

**关键设计 —— 全自动流转**：
- **构造函数自动 `startNextRound()`** 开始第 1 回合（BattleScene 创建后无需显式启动）。
- `update()` 驱动当前回合；回合 `isFinished()` 时按结果计分，未达 `winsNeeded` 则**自动 `startNextRound()`**，达到则 `MatchOver` + 记录 `m_matchWinner`。

计分规则（对应 [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) §2.4.2）：胜者 `m_roundsWon++`，平局双方都不加。

### 5.3 VictoryState（角色状态）

**文件**：[VictoryState.h](../src/states/VictoryState.h) / [VictoryState.cpp](../src/states/VictoryState.cpp)
**结构**：照搬 [DeadState](../src/states/DeadState.h)。`onEnter()` 调 `playAnimation("victory")` + `setVelocity(0,0)`，`update()` 不自动退出。

> ⚠️ **动画素材限制**：角色 config 目前**只有 `death` 动画（dead.png），没有 `victory` 动画**。`playAnimation("victory")` 找不到时保持当前动画（站立），但**状态机层面仍正确进入**（输入锁定、控制台有日志）。视觉上靠 MatchHud 的胜负横幅体现。待阶段 7 美术补素材。

### 5.4 Character::resetForRound()（新增方法）

**文件**：[Character.h](../src/entities/Character.h) / [Character.cpp](../src/entities/Character.cpp)，仅**新增** public 方法。

```cpp
void Character::resetForRound() {
    float startX = (m_playerIndex == 0) ? 300.0f : WINDOW_WIDTH - 300.0f;
    setPosition(sf::Vector2f(startX, GROUND_LEVEL));
    setVelocity(sf::Vector2f(0.0f, 0.0f));
    m_onGround = true;
    m_facingRight = (m_playerIndex == 0);

    if (m_health)  m_health->reset();    // 回满血
    if (m_stamina) m_stamina->reset();   // 回满体力 + 重置 m_wasJumping
    m_blockCooldown = false;
    m_blockCooldownTimer = 0;
    m_pendingStunFrames = 10;

    changeState(CharacterStateType::Idle);
}
```

由 RoundManager::start() 在每个回合开始时对双方各调用一次。

### 5.5 MatchHud（显示组件）

**文件**：[MatchHud.h](../src/battle/MatchHud.h) / [MatchHud.cpp](../src/battle/MatchHud.cpp)
**职责**：组合显示顶部过渡文字 + 计时 + 比分 + 胜负横幅。**阶段5 起改用 `ui/Timer` + `ui/ScoreDisplay` 组件**（过渡/横幅文字自留）。

```cpp
class MatchHud {
public:
    MatchHud();
    void update(const RoundManager& round, int roundsWonP1, int roundsWonP2,
                bool matchOver, int matchWinner);
    void render(sf::RenderWindow& window);
private:
    std::unique_ptr<Timer> m_timer;                  ///< 计时器（Fight 阶段，≤10秒红警告）
    std::unique_ptr<ScoreDisplay> m_scoreDisplay;    ///< 比分 + 胜场圆点
    std::unique_ptr<sf::Text> m_announceText;        ///< 过渡/结果/胜负横幅
};
```

显示规则：
- `matchOver && matchWinner>=0` → `PLAYER X WINS!`（胜负横幅，弥补 victory 动画缺失）
- 否则按 phase：Intro→`ROUND n`、Ready→`FIGHT!`、Fight→倒计时、RoundOver→`K.O.`/`TIME UP`/`DRAW`

> 阶段 5 做正式 UI 系统时，可拆分为 `ui/Timer` + `ui/ScoreDisplay`，MatchHud 平滑拆分。

---

## 六、对现有文件的修改点

| 文件 | 修改 | 冲突风险 |
| ---- | ---- | -------- |
| [BattleScene.h](../src/scenes/BattleScene.h) | 新增成员 `m_matchManager` + `m_matchHud` + `onMatchEnd()` 声明 | 无（纯新增） |
| [BattleScene::onEnter()](../src/scenes/BattleScene.cpp) | 角色创建后构造 MatchManager（自动开始第1回合）与 MatchHud | 在原逻辑后追加 |
| [BattleScene::update()](../src/scenes/BattleScene.cpp) | 输入传递前加 `isInputLocked()` 守卫；末尾驱动 MatchManager + `onMatchEnd()` + 刷新 MatchHud | 包裹式，不改输入/碰撞/渲染本身 |
| [BattleScene::render()](../src/scenes/BattleScene.cpp) | 末尾绘制 MatchHud | 纯追加 |
| [BattleScene::onMatchEnd()](../src/scenes/BattleScene.cpp) | MatchOver 时切 ResultScene（阶段5 增强，原直接 changeState(Victory) 已替换） | 改钩子 |
| [StateMachine::createState()](../src/states/StateMachine.cpp) | `case Victory` + `#include "VictoryState.h"` | 加一个 case |
| [Character::handleInput()](../src/entities/Character.cpp) | Dead 拦截旁加 `if (currentState == Victory) return;` | 一行守卫 |
| [Character.h/.cpp](../src/entities/Character.h) | 新增 `resetForRound()` | 纯新增 |
| [StaminaComponent::reset()](../src/components/StaminaComponent.cpp) | 补 `m_wasJumping = false;` | 一行，更彻底 |
| [CMakeLists.txt](../CMakeLists.txt) | SOURCES 追加 4 个新文件 | 见第九节 |

---

## 七、数据配置

**无需修改任何 JSON**。[game_config.json](../assets/config/game_config.json) 已含 `roundTime:60`、`winsNeeded:2`，通过 `ConfigManager` 读取，BattleScene 直接用 `Constants::ROUND_TIME` / `WINS_NEEDED`。

---

## 八、新增库与头文件

### 第三方库

**无需新增**。SFML（Graphics 文本/形状）+ nlohmann/json（已在用）完全够用。

### 新增头文件依赖清单

| 新文件 | include |
| ------ | ------- |
| `RoundManager.h` | （前向声明 Character） |
| `MatchManager.h` | `<memory>`、（前向声明 Character/RoundManager） |
| `MatchHud.h` | `<SFML/Graphics.hpp>`、`RoundManager.h` |
| `VictoryState.h` | `CharacterState.h` |
| `BattleScene.h`（改） | `../battle/MatchManager.h`、`../battle/MatchHud.h` |

全部置于 `namespace SamuraiFight`，`#pragma once`，Doxygen 注释，符合项目规范。

---

## 九、CMakeLists 改动

在 [CMakeLists.txt](../CMakeLists.txt) 的 `SOURCES` 中追加：

```cmake
    # Battle
    src/battle/RoundManager.cpp
    src/battle/MatchManager.cpp
    src/battle/MatchHud.cpp

    # States
    src/states/VictoryState.cpp
```

> 注：CMakeLists 中 `HurtState.cpp` 被列了两次（既有笔误），与本阶段无关，可另开提交清理。

---

## 十、实现里程碑

1. ✅ **VictoryState + StateMachine 接线**
2. ✅ **Character::resetForRound()**（+ StaminaComponent `m_wasJumping` 重置）
3. ✅ **RoundManager**（单回合状态机 + 胜负判定 + `isTimeUp`）
4. ✅ **MatchManager**（多回合计分 + 自动流转）
5. ✅ **MatchHud**（过渡文字 + 比分 + 胜负横幅）
6. ✅ **BattleScene 集成**（MatchManager + MatchHud + `onMatchEnd`）

---

## 十一、风险与注意事项

1. **场景切换需即时清零**：`getPopSceneCount()` / `getNextScene()` 每次 update 都被 [SceneManager](../src/managers/SceneManager.cpp) 读取，触发切换后必须立即清零，否则重复切换。
2. **双杀/同帧死亡**：`checkRoundEnd` 显式按平局处理。
3. **锁定输入的传递**：过渡阶段传"全 false"而非跳过 `handleInput`，避免角色停在攻击/跳跃中途。
4. **回合切换清理残余状态**：`resetForRound()` 的 `changeState(Idle)` 强制拉回（Hurt/Attack 的 `onExit` 无副作用，安全）。
5. **ResultScene 边界**：`onMatchEnd` 只做胜利展示，结果场景切换留给阶段 5。

---

## 十二、实测与已知限制

### 实测验证

完整三局两胜流程已跑通：`ROUND 1 → FIGHT! → 倒计时 → K.O./TIME UP → 比分更新 → ROUND 2 → ... → 先到 2 胜 → PLAYER X WINS!`。控制台日志链完整：

```
MatchManager: Starting round N (score X-Y)
RoundManager: Round N started
Character: Player X reset for new round
... (对战)
RoundManager: Round N ended (PlayerXWin)
MatchManager: Round N settled. Score X-Y
MatchManager: Player X wins the match!
VictoryState: Player X wins!
```

### 已知限制

- **VictoryState 无 victory 动画素材**：角色 config 仅有 `death`，无 `victory`。比赛结束由 ResultScene（阶段5）展示胜者，VictoryState 保留在状态机但当前 onMatchEnd 不再直接触发（改为切 ResultScene）。补 victory 动画素材属阶段 7。
- **ResultScene**（阶段5 已实现）：比赛结束 → 结果界面（胜者 + 比分 + 重赛/返回角色选择/返回主菜单）。
- **构建提示**：重新构建前需关闭运行中的游戏实例，否则链接器无法覆盖 `SamuraiFight.exe`（Windows 文件占用）。

---

## 十三、快速导航

| 想了解... | 查看位置 |
| --------- | -------- |
| 回合如何推进 | [RoundManager::update()](../src/battle/RoundManager.cpp) |
| 单回合胜负如何判定 | `RoundManager::checkRoundEnd()` |
| K.O. 与 TIME UP 如何区分 | `RoundManager::isTimeUp()` |
| 比赛如何计分/结束 | [MatchManager::update()](../src/battle/MatchManager.cpp) |
| 角色如何重置进入下一回合 | [Character::resetForRound()](../src/entities/Character.cpp) |
| 胜利状态如何接入 | [StateMachine::createState()](../src/states/StateMachine.cpp) |
| BattleScene 如何集成 | [BattleScene::update()](../src/scenes/BattleScene.cpp) / `onMatchEnd()` |
| 比赛结果显示 | [MatchHud::update()](../src/battle/MatchHud.cpp) |
| 回合/比赛参数从哪来 | [Constants.h](../src/core/Constants.h) `ROUND_TIME` / `WINS_NEEDED` |

---

**文档版本**：1.1
**最后更新**：2026年6月20日
**对应阶段**：Phase 4 - 回合与比赛
**状态**：✅ 已完成
