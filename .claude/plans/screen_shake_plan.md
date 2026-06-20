# 屏幕震动效果实现计划

## 目标
实现角色受到攻击进入硬直状态时的屏幕震动效果。

## 设计方案

### 1. 创建 ScreenShake 类
**文件**: `src/effects/ScreenShake.h` 和 `src/effects/ScreenShake.cpp`

**功能**:
- 管理震动状态（强度、持续时间、偏移）
- 提供触发震动接口
- 自动衰减震动
- 生成随机震动偏移

**核心属性**:
- `m_intensity`: 当前震动强度
- `m_duration`: 剩余震动时间
- `m_maxDuration`: 最大持续时间（用于衰减计算）
- `m_offset`: 当前震动偏移

**核心方法**:
- `trigger(float intensity, float duration)`: 触发震动
- `update(float deltaTime)`: 更新震动状态
- `getOffset() const`: 获取当前偏移
- `isActive() const`: 检查是否正在震动

### 2. 修改 BattleScene
**文件**: `src/scenes/BattleScene.h` 和 `src/scenes/BattleScene.cpp`

**修改内容**:
1. 添加 `ScreenShake` 成员变量
2. 在 `checkAttackCollision` 中检测到命中时触发震动
3. 在 `update` 中更新震动状态
4. 在 `render` 中应用震动偏移（通过 sf::View）

### 3. 震动参数配置
根据攻击类型设置不同震动强度：

| 攻击类型 | 震动强度 | 持续时间 |
|---------|---------|---------|
| Light   | 3.0     | 0.1s    |
| Medium  | 5.0     | 0.15s   |
| Heavy   | 8.0     | 0.2s    |

防御成功时：强度 6.0，持续时间 0.15s

### 4. 实现细节

**震动算法**:
```cpp
// 每帧生成随机偏移
float progress = 1.0f - (m_duration / m_maxDuration);
float currentIntensity = m_intensity * (1.0f - progress); // 线性衰减
m_offset.x = randomFloat(-currentIntensity, currentIntensity);
m_offset.y = randomFloat(-currentIntensity, currentIntensity);
```

**渲染应用**:
```cpp
// 在render开始时
sf::View view = window.getDefaultView();
view.move(screenShake.getOffset());
window.setView(view);

// 在render结束时恢复
window.setView(window.getDefaultView());
```

## 实现步骤

1. 创建 `src/effects/` 目录
2. 实现 `ScreenShake` 类
3. 修改 `BattleScene.h` 添加 ScreenShake 成员
4. 修改 `BattleScene.cpp`:
   - 在 `checkAttackCollision` 命中时触发震动
   - 在 `update` 更新震动
   - 在 `render` 应用震动偏移

## 代码风格
- 遵循项目命名规范（camelCase 变量，PascalCase 类名）
- 使用 Doxygen 注释
- 成员变量使用 `m_` 前缀
- 单个文件不超过 400 行
