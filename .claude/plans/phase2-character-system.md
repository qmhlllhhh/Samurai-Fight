# 阶段2：角色系统实现计划

> 目标：实现可在场景中移动、跳跃的角色

---

## 一、任务清单

- [ ] Character 类实现
- [ ] 状态机框架
- [ ] 基础状态（Idle, Move, Jump, Crouch）
- [ ] AnimationComponent
- [ ] JSON 角色数据加载
- [ ] HitboxComponent

---

## 二、架构设计

### 2.1 Character 组成结构

```
Character
├── StateMachine (状态机)
│   └── CharacterState* (当前状态)
│
├── Components
│   ├── AnimationComponent (动画)
│   ├── HitboxComponent (碰撞框)
│   ├── HealthComponent (生命值) - 阶段3
│   ├── StaminaComponent (体力) - 阶段3
│   └── InputBuffer (输入缓冲) - 阶段3
│
├── CharacterData (从JSON加载的数据)
│   ├── stats (属性)
│   ├── attacks (攻击数据)
│   └── animations (动画配置)
│
└── sf::Sprite (渲染精灵)
```

### 2.2 状态机架构

```
CharacterState (基类)
├── IdleState
├── MoveState
├── JumpState
├── CrouchState
├── AttackState - 阶段3
├── BlockState - 阶段3
├── HurtState - 阶段3
└── VictoryState - 阶段4
```

### 2.3 组件设计

```
AnimationComponent
├── m_spriteSheet (纹理)
├── m_frameCount (帧数)
├── m_frameDuration (每帧持续时间)
├── m_currentFrame (当前帧)
├── m_loop (是否循环)
└── playAnimation(name)
    update(deltaTime)
    render(window)

HitboxComponent
├── m_hurtboxes (受击框)
├── m_hitboxes (攻击框)
└── getHurtboxes()
    getHitboxes()
    updateHitboxes(state)
```

---

## 三、文件结构

### 3.1 需要创建的文件

```
src/entities/
├── Entity.h/cpp (实体基类)
├── Character.h/cpp (角色类)
├── CharacterData.h (角色数据结构)
└── CharacterFactory.h/cpp (角色工厂)

src/components/
├── Component.h (组件基类)
├── AnimationComponent.h/cpp
├── HitboxComponent.h/cpp

src/states/
├── CharacterState.h/cpp (状态基类)
├── IdleState.h/cpp
├── MoveState.h/cpp
├── JumpState.h/cpp
└── CrouchState.h/cpp

src/input/
├── InputManager.h/cpp (输入管理)
└── InputCommand.h (输入命令)

assets/characters/musashi/
├── config.json (角色配置)
└── idle.png, walk.png, jump.png, crouch.png (动画精灵表)
```

### 3.2 需要修改的文件

- CMakeLists.txt - 添加新源文件
- BattleScene.h/cpp - 集成角色系统
- ResourceManager.h/cpp - 添加角色资源加载方法

---

## 四、实现步骤

### 步骤1：创建基础结构

1. 创建 Entity 基类
2. 创建 Character 类框架
3. 创建 CharacterData 数据结构

### 步骤2：实现动画组件

1. 创建 AnimationComponent
2. 实现精灵表动画播放
3. 支持多动画切换

### 步骤3：实现状态机框架

1. 创建 CharacterState 基类
2. 创建 StateMachine 状态机
3. 实现状态切换逻辑

### 步骤4：实现基础状态

1. IdleState - 站立状态
2. MoveState - 移动状态（消耗体力）
3. JumpState - 跳跃状态
4. CrouchState - 下蹲状态

### 步骤5：实现碰撞框组件

1. 创建 HitboxComponent
2. 定义受击框和攻击框
3. 支持从JSON加载碰撞框数据

### 步骤6：实现输入管理

1. 创建 InputManager
2. 处理键盘输入
3. 区分玩家1和玩家2输入

### 步骤7：实现角色工厂

1. 创建 CharacterFactory
2. 从JSON加载角色数据
3. 动态创建角色实例

### 步骤8：集成到BattleScene

1. BattleScene 创建两个角色
2. 更新角色逻辑
3. 渲染角色

---

## 五、关键类设计

### 5.1 Character 类

```cpp
class Character : public Entity {
public:
    Character(const CharacterData& data, int playerIndex);
    
    void handleInput(const InputCommand& input);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    void changeState(CharacterStateType state);
    void playAnimation(const std::string& name);
    
    // 属性访问
    float getMoveSpeed() const;
    float getJumpHeight() const;
    sf::Vector2f getPosition() const;
    
private:
    CharacterData m_data;
    std::unique_ptr<StateMachine> m_stateMachine;
    std::unique_ptr<AnimationComponent> m_animation;
    std::unique_ptr<HitboxComponent> m_hitbox;
    
    sf::Sprite m_sprite;
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    
    int m_playerIndex;
    bool m_facingRight;
};
```

### 5.2 StateMachine 类

```cpp
class StateMachine {
public:
    StateMachine(Character* owner);
    
    void changeState(CharacterStateType state);
    void update(float deltaTime);
    
    CharacterState* getCurrentState() const;
    CharacterStateType getCurrentStateType() const;
    
private:
    Character* m_owner;
    std::unique_ptr<CharacterState> m_currentState;
    std::unordered_map<CharacterStateType, 
                        std::function<std::unique_ptr<CharacterState>()>> m_stateFactory;
};
```

### 5.3 AnimationComponent 类

```cpp
class AnimationComponent {
public:
    AnimationComponent(sf::Sprite& sprite);
    
    void loadAnimation(const std::string& name, 
                       const AnimationData& data);
    void play(const std::string& name);
    void update(float deltaTime);
    
    bool isFinished() const;
    std::string getCurrentAnimation() const;
    
private:
    sf::Sprite& m_sprite;
    std::unordered_map<std::string, AnimationData> m_animations;
    
    std::string m_currentAnimation;
    int m_currentFrame;
    float m_frameTimer;
    bool m_loop;
};
```

---

## 六、角色数据格式

### 6.1 CharacterData 结构

```cpp
struct CharacterData {
    std::string id;
    std::string name;
    std::string description;
    
    struct Stats {
        float maxHp;
        float maxStamina;
        float moveSpeed;
        float jumpHeight;
        float staminaRecovery;
        float moveStaminaCost;
        float jumpStaminaCost;
    } stats;
    
    std::unordered_map<std::string, AnimationData> animations;
    std::unordered_map<std::string, std::vector<HitboxData>> hitboxes;
};
```

### 6.2 JSON 配置示例 (musashi/config.json)

```json
{
  "id": "musashi",
  "name": "武藏",
  "description": "平衡型武士",
  "stats": {
    "maxHp": 100,
    "maxStamina": 100,
    "moveSpeed": 300.0,
    "jumpHeight": 400.0,
    "staminaRecovery": 0.5,
    "moveStaminaCost": 0.1,
    "jumpStaminaCost": 10.0
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
    "walk": {
      "spriteSheet": "characters/musashi/walk.png",
      "frameCount": 6,
      "frameDuration": 0.1,
      "loop": true,
      "frameWidth": 128,
      "frameHeight": 128
    },
    "jump": {
      "spriteSheet": "characters/musashi/jump.png",
      "frameCount": 4,
      "frameDuration": 0.1,
      "loop": false,
      "frameWidth": 128,
      "frameHeight": 128
    },
    "crouch": {
      "spriteSheet": "characters/musashi/crouch.png",
      "frameCount": 2,
      "frameDuration": 0.2,
      "loop": true,
      "frameWidth": 128,
      "frameHeight": 128
    }
  },
  "hitboxes": {
    "idle": [
      {"x": 40, "y": 20, "width": 48, "height": 100, "type": "hurtbox"}
    ],
    "crouch": [
      {"x": 40, "y": 60, "width": 48, "height": 60, "type": "hurtbox"}
    ]
  }
}
```

---

## 七、输入映射

### 7.1 InputCommand 结构

```cpp
enum class InputAction {
    MoveLeft,
    MoveRight,
    Jump,
    Crouch,
    AttackLight,
    AttackMedium,
    AttackHeavy,
    AttackSpecial,
    Block
};

struct InputCommand {
    InputAction action;
    bool pressed;  // true=按下, false=释放
    int frame;     // 输入帧号
};
```

### 7.2 键位映射

从 keybindings.json 加载，InputManager 负责：
- 查询键盘状态
- 生成 InputCommand
- 区分玩家1和玩家2

---

## 八、物理模拟

### 8.1 移动

```cpp
// MoveState::update()
m_velocity.x = m_direction * m_character->getMoveSpeed();
m_character->move(m_velocity * deltaTime);
```

### 8.2 跳跃

```cpp
// JumpState::onEnter()
m_velocity.y = -m_character->getJumpHeight();

// JumpState::update()
m_velocity.y += GRAVITY * deltaTime;  // 重力
m_character->move(m_velocity * deltaTime);

// 检测落地
if (m_character->getPosition().y >= GROUND_LEVEL) {
    m_character->changeState(CharacterStateType::Idle);
}
```

### 8.3 重力

```cpp
// Constants.h
constexpr float GRAVITY = 800.0f;      // 重力加速度
constexpr float GROUND_LEVEL = 500.0f; // 地面Y坐标
```

---

## 九、渲染顺序

BattleScene::render() 渲染顺序：
1. 背景层
2. 角色1
3. 角色2
4. UI层（血条、计时器等）

---

## 十、测试验证

### 10.1 功能测试

- 角色可以站立、移动、跳跃、下蹲
- 动画正确播放
- 状态切换流畅
- 体力消耗和恢复正确
- 碰撞框数据加载正确

### 10.2 性能测试

- 60 FPS 稳定运行
- 内存占用正常
- 无明显延迟

---

## 十一、文件创建顺序

1. Constants.h - 添加物理常量
2. entities/Entity.h/cpp
3. entities/CharacterData.h
4. components/Component.h
5. components/AnimationComponent.h/cpp
6. components/HitboxComponent.h/cpp
7. states/CharacterState.h/cpp
8. states/IdleState.h/cpp
9. states/MoveState.h/cpp
10. states/JumpState.h/cpp
11. states/CrouchState.h/cpp
12. entities/StateMachine.h/cpp
13. input/InputCommand.h
14. input/InputManager.h/cpp
15. entities/Character.h/cpp
16. entities/CharacterFactory.h/cpp
17. BattleScene.cpp - 集成角色
18. assets/characters/musashi/config.json
19. CMakeLists.txt - 添加源文件

---

## 十二、注意事项

1. SFML 3.0 API 变化
   - Sprite 构造需要纹理参数
   - 事件处理使用 `std::optional`
   
2. 资源管理
   - 使用 ResourceManager 单例
   - 纹理使用 smooth 参数
   
3. 状态管理
   - Scene 使用 m_popSceneCount 而非 m_finished
   - 场景切换逻辑已更新

4. 动画资源
   - 需要创建或下载像素风格精灵表
   - 精灵表应为横向排列帧

---

**预计工作量**：阶段2 需要约 20-30 个文件的创建和修改