# 跑步状态实现计划

## 需求分析

1. **跑步**：按住A/D键即可触发（不需要按S键）
2. **走路**：需要按住S + A/D键触发
3. 跑步期间消耗体力值
4. 跑步比走路快，比翻滚慢
5. 跑步期间有受击框
6. 各角色跑步动画为run.png

## 实现方案

### 方案概述

1. 将现有 `MoveState` 重命名为 `WalkState`（走路状态）
2. 新增 `RunState`（跑步状态）
3. 修改输入处理：
   - 方向键 = 跑步
   - S + 方向键 = 走路

### 文件修改清单

#### 1. 新建文件
- `src/states/RunState.h` - 跑步状态头文件
- `src/states/RunState.cpp` - 跑步状态实现

#### 2. 重命名文件
- `MoveState.h` → `WalkState.h`
- `MoveState.cpp` → `WalkState.cpp`

#### 3. 修改文件
- `src/states/CharacterStateType.h` - `Move` 改为 `Walk`，添加 `Run`
- `src/states/StateMachine.cpp` - 更新状态创建逻辑
- `src/input/InputCommand.h` - 添加 `runLeft/runRight`
- `src/input/InputManager.cpp` - 区分走路和跑步输入
- `src/entities/Character.cpp` - 更新状态处理
- `CMakeLists.txt` - 更新源文件

### 详细实现

#### 1. CharacterStateType.h
```cpp
enum class CharacterStateType {
    Idle,
    Walk,           // Move 改为 Walk
    Run,            // 新增：跑步
    Jump,
    // ...
};
```

#### 2. RunState 类设计
```cpp
class RunState : public CharacterState {
public:
    RunState(Character* owner);
    ~RunState() override;

    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;

private:
    int m_direction;              // 跑步方向
    float m_runSpeed;             // 跑步速度
    float m_staminaCostPerFrame;  // 每帧体力消耗
};
```

#### 3. 速度设定
- 走路速度：`moveSpeed`（角色数据）
- 跑步速度：`moveSpeed * 1.8f`（比走路快80%）
- 翻滚速度：`1000.0f`（最快）

#### 4. InputManager.cpp 修改
```cpp
// 走路：S + 方向键
inputState.walkLeft = downPressed && leftPressed;
inputState.walkRight = downPressed && rightPressed;

// 跑步：仅方向键（不按S）
inputState.runLeft = !downPressed && leftPressed;
inputState.runRight = !downPressed && rightPressed;
```

#### 5. Character.cpp 输入处理优先级
```
1. 死亡/受击/翻滚 - 不接受输入
2. 防御状态处理
3. 攻击状态处理
4. 地面攻击
5. 翻滚（需要体力）
6. 跳跃（需要体力）
7. 跑步（需要体力）- 新增
8. 走路（不需要体力阈值）
```

#### 6. 体力消耗
- 跑步消耗：0.3 体力/帧
- 走路消耗：0.1 体力/帧
- 翻滚消耗：15 体力（一次性）

### 实现步骤

1. ✅ 分析现有代码结构
2. 修改 `InputCommand.h` - 添加 `runLeft/runRight`，保留 `moveLeft/moveRight` 用于走路
3. 修改 `InputManager.cpp` - 区分走路和跑步输入
4. 修改 `CharacterStateType.h` - `Move` → `Walk`，添加 `Run`
5. 重命名 `MoveState` → `WalkState`
6. 创建 `RunState.h/cpp` - 实现跑步状态
7. 修改 `StateMachine.cpp` - 注册 `WalkState` 和 `RunState`
8. 修改 `Character.cpp` - 添加 `Run` 状态逻辑
9. 修改 `CMakeLists.txt` - 更新源文件
