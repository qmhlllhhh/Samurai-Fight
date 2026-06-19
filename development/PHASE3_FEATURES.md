# Phase 3 新增功能说明

> 本文档说明相较于 Phase 2 新增的功能逻辑，帮助协作者快速了解代码结构。

---

## 目录

- [概述](#概述)
- [攻击系统](#攻击系统)
- [生命值系统](#生命值系统)
- [体力值系统](#体力值系统)
- [受击状态](#受击状态)
- [碰撞检测](#碰撞检测)
- [UI系统](#ui系统)
- [边界限制](#边界限制)
- [配置文件说明](#配置文件说明)

---

## 概述

Phase 3 主要实现了战斗系统的核心功能，包括：

| 功能模块 | 主要文件 |
|---------|---------|
| 攻击系统 | `AttackState.h/cpp` |
| 生命值系统 | `HealthComponent.h/cpp` |
| 体力值系统 | `StaminaComponent.h/cpp` |
| 受击状态 | `HurtState.h/cpp` |
| UI显示 | `HealthBar.h/cpp`, `StaminaBar.h/cpp` |

---

## 攻击系统

### 状态类型

新增三种攻击状态（[CharacterStateType.h](../src/states/CharacterStateType.h)）：

```cpp
enum class CharacterStateType {
    // ... 原有状态 ...
    AttackLight,    // 轻攻击
    AttackMedium,   // 中攻击
    AttackHeavy,    // 重攻击
    Hurt,           // 受击
    // ...
};
```

### 攻击帧数据

攻击数据结构（[CharacterData.h](../src/entities/CharacterData.h)）：

```cpp
struct AttackData {
    float damage;           // 伤害值
    int startupFrames;      // 前摇帧数
    int activeFrames;       // 判定帧数
    int recoveryFrames;     // 后摇帧数
    int stunFrames;         // 对手硬直帧数
    bool canCancel;         // 是否可取消
    int cancelWindowStart;  // 取消窗口开始帧
    int cancelWindowEnd;    // 取消窗口结束帧
    int hitCount;           // 段数（最大命中次数）
};
```

### 攻击流程

```
按下攻击键 → handleInput() 检测
                ↓
        切换到 AttackState
                ↓
    ┌─────────────────────────────┐
    │ 前摇 (startupFrames)        │ ← 可取消窗口
    ├─────────────────────────────┤
    │ 判定 (activeFrames)         │ ← 攻击框激活
    ├─────────────────────────────┤
    │ 后摇 (recoveryFrames)       │
    └─────────────────────────────┘
                ↓
        回到 Idle 状态
```

### 关键代码位置

| 功能 | 文件 | 方法 |
|------|------|------|
| 攻击输入处理 | [Character.cpp](../src/entities/Character.cpp) | `handleInput()` |
| 攻击状态逻辑 | [AttackState.cpp](../src/states/AttackState.cpp) | `update()` |
| 取消检测 | [AttackState.cpp](../src/states/AttackState.cpp) | `canCancel()` |
| 判定帧检测 | [AttackState.cpp](../src/states/AttackState.cpp) | `isInActiveFrames()` |

---

## 生命值系统

### 组件结构

[HealthComponent.h](../src/components/HealthComponent.h)：

```cpp
class HealthComponent {
public:
    float takeDamage(float damage);    // 受到伤害
    float heal(float amount);          // 恢复生命
    float getCurrentHp() const;        // 获取当前生命值
    float getHpPercent() const;        // 获取生命值百分比
    bool isDead() const;               // 检查是否死亡
    void reset();                      // 重置到最大值
};
```

### 集成到角色

[Character.h](../src/entities/Character.h)：

```cpp
class Character {
    std::unique_ptr<HealthComponent> m_health;  // 生命值组件
    
public:
    void takeDamage(float damage, int stunFrames);  // 受伤并触发硬直
    HealthComponent* getHealthComponent();
    bool isDead() const;
};
```

### 伤害流程

```
攻击命中 → BattleScene::checkAttackCollision()
                ↓
        检测攻击框与受击框相交
                ↓
        调用 defender->takeDamage(damage, stunFrames)
                ↓
        HealthComponent::takeDamage() 扣减生命值
                ↓
        触发 HurtState 受击状态
```

---

## 体力值系统

### 组件结构

[StaminaComponent.h](../src/components/StaminaComponent.h)：

```cpp
class StaminaComponent {
public:
    float consume(float amount);       // 消耗体力
    float recover(float amount);       // 恢复体力
    void update(...);                  // 每帧更新
    bool isExhausted() const;          // 检查是否耗尽
    bool hasEnoughStamina(float) const;
};
```

### 体力消耗规则

| 状态 | 体力变化 | 配置字段 |
|------|---------|---------|
| 站立 | +0.5/帧 | `staminaRecovery` |
| 移动 | -0.1/帧 | `moveStaminaCost` |
| 跳跃 | -10（开始时） | `jumpStaminaCost` |

### 更新逻辑

[Character.cpp](../src/entities/Character.cpp) `update()`：

```cpp
if (m_stamina) {
    bool isMoving = (getCurrentStateType() == CharacterStateType::Move);
    bool isJumping = (getCurrentStateType() == CharacterStateType::Jump);
    bool isStanding = (getCurrentStateType() == CharacterStateType::Idle && m_onGround);
    
    m_stamina->update(isMoving, isJumping, isStanding, ...);
}
```

---

## 受击状态

### 状态结构

[HurtState.h](../src/states/HurtState.h)：

```cpp
class HurtState : public CharacterState {
public:
    HurtState(Character* owner, int stunFrames);
    void onEnter() override;   // 播放受击动画，停止移动
    void update(float deltaTime) override;  // 硬直计时
    bool isStunFinished() const;
};
```

### 受击流程

```
受到伤害 → Character::takeDamage()
                ↓
        保存硬直帧数 → m_pendingStunFrames
                ↓
        切换状态 → changeState(CharacterStateType::Hurt)
                ↓
        HurtState::onEnter()
                ↓
        播放 "hurt" 动画
                ↓
        硬直期间不接受任何输入
                ↓
        硬直结束 → 回到 Idle 状态
```

### 输入阻断

[Character.cpp](../src/entities/Character.cpp) `handleInput()`：

```cpp
if (currentState == CharacterStateType::Hurt) {
    return;  // 受击状态下不接受任何输入
}
```

---

## 碰撞检测

### 碰撞框类型

| 类型 | 说明 | 颜色（调试显示） |
|------|------|-----------------|
| hurtbox | 受击框（角色身体） | 绿色 |
| hitbox | 攻击框（攻击判定） | 红色 |

### 检测流程

[BattleScene.cpp](../src/scenes/BattleScene.cpp)：

```cpp
void BattleScene::checkCollisions() {
    checkAttackCollision(0, 1);  // 玩家1攻击玩家2
    checkAttackCollision(1, 0);  // 玩家2攻击玩家1
}

void BattleScene::checkAttackCollision(int attacker, int defender) {
    // 1. 检查攻击者是否在攻击状态
    // 2. 检查是否还能命中（段数限制）
    // 3. 获取攻击者的攻击框
    // 4. 获取防御者的受击框
    // 5. 检测碰撞框是否相交
    // 6. 命中则造成伤害并增加命中计数
}
```

### 段数限制

一次攻击在判定帧期间最多命中 `hitCount` 次：

```cpp
// AttackState
bool canHit() const { return m_currentHitCount < m_maxHitCount; }
void incrementHitCount() { m_currentHitCount++; }
```

---

## UI系统

### 血条

[HealthBar.h](../src/ui/HealthBar.h)：

```cpp
class HealthBar {
public:
    void update(float currentHp, float maxHp);  // 更新显示
    void render(sf::RenderWindow& window);
};
```

颜色规则：
- 绿色：生命值 > 50%
- 黄色：生命值 25% ~ 50%
- 红色：生命值 < 25%

### 体力条

[StaminaBar.h](../src/ui/StaminaBar.h)：

```cpp
class StaminaBar {
public:
    void update(float currentStamina, float maxStamina);
    void render(sf::RenderWindow& window);
};
```

颜色规则：
- 蓝色：体力值 > 30%
- 橙色：体力值 < 30%

### UI布局

```
┌─────────────────────────────────────────────────┐
│ [P1血条 300x25]                    [P2血条]    │
│ [P1体力条 300x12]                  [P2体力条]  │
│                                                  │
│              [战斗区域]                          │
│                                                  │
└─────────────────────────────────────────────────┘
```

---

## 边界限制

### 实现位置

[Character.cpp](../src/entities/Character.cpp)：

```cpp
void Character::checkScreenBounds() {
    // 1. 获取当前状态的碰撞框
    // 2. 计算角色的实际边界（基于受击框）
    // 3. 限制在屏幕范围内
    //    - 左边界：minX >= 0
    //    - 右边界：maxX <= WINDOW_WIDTH
}
```

### 调用时机

在 `Character::update()` 中，位置更新后立即检测：

```cpp
m_position += m_velocity * deltaTime;
checkGroundCollision();
checkScreenBounds();  // 屏幕边界检测
```

---

## 配置文件说明

### 角色配置结构

`assets/characters/<name>/config.json`：

```json
{
    "stats": {
        "maxHp": 100,
        "maxStamina": 100,
        "staminaRecovery": 0.5,
        "moveStaminaCost": 0.1,
        "jumpStaminaCost": 10.0
    },
    "attacks": {
        "light": {
            "damage": 10,
            "startupFrames": 3,
            "activeFrames": 2,
            "recoveryFrames": 5,
            "stunFrames": 8,
            "canCancel": false,
            "hitCount": 1
        },
        "medium": { ... },
        "heavy": { ... }
    },
    "animations": {
        "attack_light": { "spriteSheet": ".../attack_1.png", ... },
        "attack_medium": { "spriteSheet": ".../attack_2.png", ... },
        "attack_heavy": { "spriteSheet": ".../attack_3.png", ... },
        "hurt": { "spriteSheet": ".../hurt.png", ... }
    },
    "hitboxes": {
        "attack_light": [
            { "type": "hurtbox", ... },
            { "type": "hitbox", "activeFrames": [3, 4], ... }
        ],
        "hurt": [ { "type": "hurtbox", ... } ]
    }
}
```

### 攻击帧数据说明

| 字段 | 说明 | 轻攻击 | 中攻击 | 重攻击 |
|------|------|--------|--------|--------|
| damage | 伤害值 | 10 | 15 | 20 |
| startupFrames | 前摇帧 | 3 | 5 | 8 |
| activeFrames | 判定帧 | 2 | 3 | 4 |
| recoveryFrames | 后摇帧 | 5 | 8 | 12 |
| stunFrames | 硬直帧 | 8 | 12 | 18 |
| hitCount | 段数 | 1 | 2 | 3 |

### 碰撞框 activeFrames

`activeFrames` 指定攻击框在哪些帧激活：

```json
{
    "type": "hitbox",
    "activeFrames": [3, 4]  // 第3、4帧时攻击框生效
}
```

---

## 调试功能

### 碰撞框显示

按 **F1** 切换碰撞框显示：

- 绿色半透明：受击框
- 红色半透明：攻击框（仅在 activeFrames 期间显示）

### 控制台输出

命中时输出：
```
BattleScene: Player 1 hits Player 2 for 10 damage! (Hit 1/1)
Character 樱 takes 10 damage (HP: 90/100), stun for 8 frames
HurtState: Entered with 8 frames stun
```

---

## 快速导航

| 想了解... | 查看文件 |
|----------|---------|
| 攻击如何触发 | [Character.cpp:handleInput()](../src/entities/Character.cpp) |
| 攻击帧逻辑 | [AttackState.cpp](../src/states/AttackState.cpp) |
| 伤害如何计算 | [BattleScene.cpp:checkAttackCollision()](../src/scenes/BattleScene.cpp) |
| 生命值管理 | [HealthComponent.cpp](../src/components/HealthComponent.cpp) |
| 体力值消耗 | [StaminaComponent.cpp](../src/components/StaminaComponent.cpp) |
| 受击硬直 | [HurtState.cpp](../src/states/HurtState.cpp) |
| 碰撞框更新 | [HitboxComponent.cpp](../src/components/HitboxComponent.cpp) |
| UI显示 | [HealthBar.cpp](../src/ui/HealthBar.cpp), [StaminaBar.cpp](../src/ui/StaminaBar.cpp) |

---

**文档版本**：1.0  
**最后更新**：2026年6月19日  
**对应阶段**：Phase 3 - 战斗系统
