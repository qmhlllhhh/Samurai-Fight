# 阶段2：角色系统 - 新增功能概览

> 完成日期：2026年6月19日
> 状态：✅ 已完成

---

## 一、相较于阶段1的主要变化

### 1.1 新增核心系统

| 系统         | 说明               | 主要文件                        |
| ------------ | ------------------ | ------------------------------- |
| **实体系统** | 游戏对象基类       | `src/entities/Entity.h/cpp`     |
| **角色系统** | 玩家控制的战斗角色 | `src/entities/Character.h/cpp`  |
| **状态机**   | 角色行为状态管理   | `src/states/StateMachine.h/cpp` |
| **组件系统** | 可复用的功能组件   | `src/components/`               |
| **输入缓冲** | 帧精确输入记录     | `src/input/InputBuffer.h/cpp`   |

### 1.2 目录结构变化

```
src/
├── entities/        ← 新增：实体和角色
│   ├── Entity.h/cpp
│   ├── Character.h/cpp
│   ├── CharacterData.h/cpp
│   └── CharacterFactory.h/cpp
│
├── components/      ← 新增：组件系统
│   ├── AnimationComponent.h/cpp
│   └── HitboxComponent.h/cpp
│
├── states/          ← 新增：状态机
│   ├── CharacterState.h/cpp
│   ├── StateMachine.h/cpp
│   ├── IdleState.h/cpp
│   ├── MoveState.h/cpp
│   ├── JumpState.h/cpp
│   └── CrouchState.h/cpp
│
├── input/           ← 新增：输入系统
│   ├── InputCommand.h
│   ├── InputManager.h/cpp
│   └── InputBuffer.h/cpp
│
└── scenes/
    └── BattleScene.cpp  ← 更新：集成角色系统
```

---

## 二、实体与角色系统

### 2.1 Entity 基类

**职责**：所有游戏对象的基类

```cpp
class Entity {
public:
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& position);
    sf::Vector2f getVelocity() const;
    void setVelocity(const sf::Vector2f& velocity);
    bool isOnGround() const;
    
protected:
    sf::Vector2f m_position;  // 位置
    sf::Vector2f m_velocity;  // 速度
    bool m_onGround;          // 是否在地面
};
```

**关键特性**：
- 提供位置、速度的统一管理
- 地面碰撞检测 (`m_onGround`)
- 为所有游戏对象提供统一接口

### 2.2 Character 类

**职责**：玩家控制的角色

```cpp
class Character : public Entity {
public:
    Character(const CharacterData& data, int playerIndex);
    
    void handleInput(...);      // 处理输入
    void update(float deltaTime);  // 更新逻辑
    void render(sf::RenderWindow& window);  // 渲染
    
    void changeState(CharacterStateType state);  // 切换状态
    void playAnimation(const std::string& name); // 播放动画
    
    InputBuffer& getInputBuffer();  // 获取输入缓冲
    
private:
    CharacterData m_data;              // 角色数据
    std::unique_ptr<StateMachine> m_stateMachine;
    std::unique_ptr<AnimationComponent> m_animation;
    std::unique_ptr<HitboxComponent> m_hitbox;
    std::unique_ptr<InputBuffer> m_inputBuffer;
    
    int m_frameCount;    // 帧计数器
    bool m_facingRight;  // 朝向
};
```

**关键特性**：
- 组合多个组件（动画、碰撞框、输入缓冲）
- 持有状态机管理行为
- 从 JSON 加载角色数据

### 2.3 CharacterData 数据结构

```cpp
struct CharacterData {
    std::string id;
    std::string name;
    StatsData stats;  // 属性数据
    
    std::unordered_map<std::string, AnimationData> animations;
    std::unordered_map<std::string, std::vector<HitboxData>> hitboxes;
    
    bool loadFromJson(const std::string& jsonPath);
};
```

**数据加载流程**：
```
config.json → CharacterData::loadFromJson() → CharacterFactory 缓存
```

### 2.4 CharacterFactory 工厂类

**职责**：创建角色实例

```cpp
class CharacterFactory {
public:
    static CharacterFactory& getInstance();
    
    bool initialize();  // 加载所有角色数据
    bool loadCharacterData(const std::string& id, const std::string& path);
    std::unique_ptr<Character> createCharacter(const std::string& id, int playerIndex);
    
private:
    std::unordered_map<std::string, CharacterData> m_characterData;
};
```

**使用方式**：
```cpp
// 在 Game::initialize() 中初始化
CharacterFactory::getInstance().initialize();

// 在 BattleScene 中创建角色
auto character = factory.createCharacter("musashi", 0);
```

---

## 三、状态机系统

### 3.1 架构设计

```
CharacterState (基类)
├── IdleState      - 站立
├── MoveState      - 移动
├── JumpState      - 跳跃
├── CrouchState    - 下蹲
├── AttackState    - 攻击（阶段3）
├── BlockState     - 防御（阶段3）
└── HurtState      - 受击（阶段3）
```

### 3.2 CharacterState 基类

```cpp
class CharacterState {
public:
    CharacterState(Character* owner, CharacterStateType type);
    virtual ~CharacterState();
    
    virtual void onEnter();   // 进入状态
    virtual void onExit();    // 退出状态
    virtual void update(float deltaTime);  // 更新状态
    
    CharacterStateType getType() const;
    
protected:
    Character* m_owner;       // 所属角色
    int m_frameCount;         // 当前帧计数
};
```

### 3.3 StateMachine 状态机

```cpp
class StateMachine {
public:
    StateMachine(Character* owner);
    
    void changeState(CharacterStateType stateType);
    void update(float deltaTime);
    
    CharacterStateType getCurrentStateType() const;
    
private:
    Character* m_owner;
    std::unique_ptr<CharacterState> m_currentState;
};
```

**状态切换流程**：
```
Character::changeState(Idle)
    ↓
StateMachine::changeState(Idle)
    ├── 旧状态.onExit()
    ├── 创建新状态实例
    └── 新状态.onEnter()
```

### 3.4 具体状态实现

#### IdleState - 站立状态
```cpp
void IdleState::onEnter() {
    m_owner->playAnimation("idle");
    m_owner->setVelocity(sf::Vector2f(0, m_owner->getVelocity().y));
}
```

#### MoveState - 移动状态
```cpp
void MoveState::update(float deltaTime) {
    int direction = m_owner->isFacingRight() ? 1 : -1;
    float moveSpeed = m_owner->getMoveSpeed();
    m_owner->setVelocity(sf::Vector2f(direction * moveSpeed, ...));
}
```

#### JumpState - 跳跃状态
```cpp
void JumpState::onEnter() {
    m_owner->playAnimation("jump");
    float jumpHeight = m_owner->getJumpHeight();
    m_owner->setVelocity(sf::Vector2f(..., -jumpHeight));
}

void JumpState::update(float deltaTime) {
    // 空中移动控制
    if (m_owner->isOnGround()) {
        m_owner->changeState(CharacterStateType::Idle);
    }
}
```

#### CrouchState - 下蹲状态
```cpp
void CrouchState::onEnter() {
    m_owner->playAnimation("crouch");
    m_owner->setVelocity(sf::Vector2f(0, ...));
}
```

---

## 四、组件系统

### 4.1 组件设计理念

**组合优于继承**：角色由多个独立组件组成，每个组件负责单一功能。

### 4.2 AnimationComponent

**职责**：管理精灵表动画播放

```cpp
class AnimationComponent {
public:
    AnimationComponent(sf::Sprite& sprite);
    
    bool loadAnimation(const std::string& name, const AnimationData& data);
    void play(const std::string& name);
    void update(float deltaTime);
    
    bool isFinished() const;
    std::string getCurrentAnimation() const;
    int getCurrentFrame() const;
    
private:
    sf::Sprite& m_sprite;
    std::unordered_map<std::string, AnimationData> m_animations;
    
    std::string m_currentAnimation;
    int m_currentFrame;
    float m_frameTimer;
};
```

**动画数据格式**：
```cpp
struct AnimationData {
    std::string spriteSheet;  // 精灵表路径
    int frameCount;           // 帧数
    float frameDuration;      // 每帧持续时间
    bool loop;                // 是否循环
    int frameWidth;           // 帧宽度
    int frameHeight;          // 帧高度
};
```

**动画播放流程**：
```
Character::playAnimation("walk")
    ↓
AnimationComponent::play("walk")
    ├── 设置纹理
    ├── 重置帧计数
    └── 设置精灵区域

AnimationComponent::update(deltaTime)
    ├── 累积时间
    ├── 判断是否切换帧
    └── 更新精灵纹理区域
```

### 4.3 HitboxComponent

**职责**：管理碰撞框（受击框和攻击框）

```cpp
struct Hitbox {
    sf::FloatRect rect;       // 碰撞矩形
    std::string type;         // "hurtbox" 或 "hitbox"
    std::vector<int> activeFrames;  // 激活帧
};

class HitboxComponent {
public:
    void loadHitboxes(const std::string& state, const std::vector<HitboxData>& data);
    void update(const sf::Vector2f& position, const std::string& state, 
                int frame, bool facingRight);
    
    std::vector<Hitbox> getHurtboxes() const;
    std::vector<Hitbox> getHitboxes() const;
    
    void renderDebug(sf::RenderWindow& window);  // 调试渲染
};
```

**碰撞框类型**：
- **hurtbox**: 受击框，角色可被攻击的区域
- **hitbox**: 攻击框，角色攻击判定的区域

---

## 五、输入缓冲系统

### 5.1 设计目的

**问题**：格斗游戏需要帧精确的输入检测
- 连招系统需要检测特定帧范围内的输入
- 攻击取消窗口需要检测特定输入
- 区分"按下瞬间"和"持续按住"

### 5.2 InputBuffer 结构

```cpp
struct BufferedInput {
    InputAction action;  // 输入动作
    bool pressed;        // true=按下, false=释放
    int frame;           // 输入发生的帧号
};

class InputBuffer {
public:
    explicit InputBuffer(int bufferSize = 12);  // 默认12帧
    
    void pushInput(InputAction action, bool pressed, int frame);
    bool hasInput(InputAction action, int startFrame, int endFrame) const;
    bool hasPressEvent(InputAction action, int startFrame, int endFrame) const;
    
    void setCurrentFrame(int frame);
    
private:
    std::deque<BufferedInput> m_buffer;
    int m_currentFrame;
};
```

### 5.3 使用示例

```cpp
// 在 Character 中更新输入缓冲
void Character::updateInputBuffer() {
    m_inputBuffer->setCurrentFrame(m_frameCount);
    
    if (m_attackLight) {
        m_inputBuffer->pushInput(InputAction::AttackLight, true, m_frameCount);
    }
}

// 检测取消窗口
if (m_inputBuffer->hasPressEvent(InputAction::AttackMedium, 0, 3)) {
    // 过去3帧内有中攻击按下
}
```

### 5.4 InputManager vs InputBuffer

| 特性     | InputManager     | InputBuffer    |
| -------- | ---------------- | -------------- |
| 用途     | 实时按键状态     | 帧精确输入历史 |
| 调用者   | BattleScene      | Character      |
| 功能     | 查询当前按键状态 | 记录输入历史   |
| 适用场景 | 移动、跳跃       | 连招、取消窗口 |

**协作流程**：
```
InputManager::update()
    ↓ 获取实时按键状态
BattleScene::update()
    ↓ 传递给角色
Character::handleInput(移动, 跳跃, 攻击)
    ↓ 保存输入状态
Character::updateInputBuffer()
    ↓ 记录攻击输入
InputBuffer::pushInput()
```

---

## 六、BattleScene 集成

### 6.1 角色创建流程

```cpp
void BattleScene::initializeCharacters() {
    // 获取工厂并初始化（已由 Game 调用）
    CharacterFactory& factory = CharacterFactory::getInstance();
    
    // 创建两个角色
    m_characters[0] = factory.createCharacter("musashi", 0);  // 玩家1
    m_characters[1] = factory.createCharacter("sakura", 1);   // 玩家2
}
```

### 6.2 更新流程

```cpp
void BattleScene::update(float deltaTime) {
    // 1. 更新输入管理器
    m_inputManager->update(*m_window);
    
    // 2. 传递输入给角色
    for (int i = 0; i < 2; ++i) {
        InputState input = m_inputManager->getPlayerInput(i);
        m_characters[i]->handleInput(
            input.moveLeft, input.moveRight, input.jump, input.crouch,
            input.attackLight, input.attackMedium, input.attackHeavy
        );
    }
    
    // 3. 更新角色
    for (int i = 0; i < 2; ++i) {
        m_characters[i]->update(deltaTime);
    }
}
```

### 6.3 渲染流程

```cpp
void BattleScene::render(sf::RenderWindow& window) {
    // 绘制背景
    if (m_background) window.draw(*m_background);
    
    // 绘制角色
    for (int i = 0; i < 2; ++i) {
        m_characters[i]->render(window);
    }
}
```

---

## 七、角色配置文件

### 7.1 JSON 结构

```json
{
    "id": "musashi",
    "name": "武藏",
    "stats": {
        "maxHp": 100,
        "moveSpeed": 300.0,
        "jumpHeight": 400.0,
        ...
    },
    "animations": {
        "idle": {
            "spriteSheet": "characters/musashi/idle.png",
            "frameCount": 4,
            "frameDuration": 0.133,
            "loop": true,
            "frameWidth": 128,
            "frameHeight": 128
        },
        ...
    },
    "hitboxes": {
        "idle": [
            {"x": -24, "y": -108, "width": 48, "height": 100, "type": "hurtbox"}
        ]
    }
}
```

### 7.2 已有角色

| 角色 | ID        | 定位   | 文件位置                     |
| ---- | --------- | ------ | ---------------------------- |
| 武藏 | `musashi` | 平衡型 | `assets/characters/musashi/` |
| 樱   | `sakura`  | 敏捷型 | `assets/characters/sakura/`  |

---

## 八、关键更新逻辑

### 8.1 Character::update 执行顺序

```cpp
void Character::update(float deltaTime) {
    m_frameCount++;                    // 1. 帧计数
    updateInputBuffer();               // 2. 更新输入缓冲
    applyGravity(deltaTime);           // 3. 应用重力
    m_position += m_velocity * deltaTime;  // 4. 更新位置
    checkGroundCollision();            // 5. 检测地面
    m_stateMachine->update(deltaTime); // 6. 更新状态机
    m_animation->update(deltaTime);    // 7. 更新动画
    // ... 更新碰撞框和精灵
}
```

### 8.2 Character::handleInput 状态处理

```cpp
void Character::handleInput(...) {
    // 保存输入状态
    
    // 跳跃状态：空中控制
    if (currentState == Jump) {
        // 更新朝向
        // 设置空中移动速度
        return;
    }
    
    // 地面状态：
    // - 跳跃检测
    // - 下蹲检测
    // - 移动检测
    // - 状态切换
}
```

---

## 九、与阶段1的对比

### 9.1 代码量变化

| 阶段  | 文件数 | 主要功能               |
| ----- | ------ | ---------------------- |
| 阶段1 | ~15    | 框架、主菜单           |
| 阶段2 | ~35    | 角色系统、状态机、组件 |

**新增文件**：约 20 个

### 9.2 复杂度提升

| 方面     | 阶段1    | 阶段2                   |
| -------- | -------- | ----------------------- |
| 架构模式 | 单例     | 单例 + 组合 + 状态机    |
| 输入处理 | 实时状态 | 实时状态 + 帧缓冲       |
| 渲染对象 | 简单精灵 | 组件化角色              |
| 数据驱动 | 配置文件 | 配置文件 + JSON角色数据 |

### 9.3 新增设计模式

| 模式         | 应用场景                                |
| ------------ | --------------------------------------- |
| **组合模式** | Character 由多个 Component 组成         |
| **状态模式** | StateMachine 管理角色行为状态           |
| **工厂模式** | CharacterFactory 创建角色实例           |
| **单例模式** | Game, ResourceManager, CharacterFactory |

---

## 十、后续开发建议

### 10.1 阶段3 - 战斗系统

需要实现：
- `AttackState` 攻击状态
- 攻击取消窗口逻辑
- 连招系统
- 碰撞检测
- 体力槽系统

### 10.2 扩展角色

1. 在 `assets/characters/` 创建角色目录
2. 创建 `config.json` 配置文件
3. 准备动画精灵表
4. 在 `CharacterFactory::initialize()` 中加载

### 10.3 调试技巧

```cpp
// 开启调试信息
m_showDebug = true;  // 显示位置、速度、状态

// 查看输入状态
InputState input = m_inputManager->getPlayerInput(0);

// 检查角色状态
CharacterStateType state = m_characters[0]->getCurrentStateType();
```

---

**文档维护者**：lllhhh
**最后更新**：2026年6月19日
