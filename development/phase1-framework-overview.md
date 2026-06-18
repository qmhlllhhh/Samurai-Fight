# 阶段1：项目框架 - 技术文档

> 完成日期：2026年6月18日
> 状态：✅ 已完成

---

## 一、概述

阶段1完成了游戏的核心框架搭建，实现了可运行的主菜单窗口。采用固定时间步长游戏循环、场景栈管理、资源单例缓存等架构模式。

### 交付物

- ✅ 可运行的主菜单窗口 (1280×720)
- ✅ 键盘导航 (上下箭头 + Enter)
- ✅ 鼠标交互 (悬停 + 点击)
- ✅ ESC 退出游戏

---

## 二、架构设计

### 2.1 核心架构图

```
Game (单例)
├── m_window (sf::RenderWindow)
├── m_sceneManager (SceneManager)
├── m_configManager (ConfigManager)
├── m_frameTimer (FrameTimer)
└── ResourceManager::getInstance() (单例)

SceneManager
└── m_sceneStack (std::vector<std::unique_ptr<Scene>>)
    └── MainMenuScene (当前)
```

### 2.2 游戏循环流程

```
Game::run()
  │
  ├── frameTimer.update()           // 更新累积时间
  │
  └── while (frameTimer.shouldUpdate())
        │
        ├── handleEvents()          // 处理窗口事件
        │     └── SceneManager::handleEvents()
        │           └── Scene::handleEvents()
        │
        ├── update(FRAME_TIME)      // 固定 1/60 秒更新
        │     └── SceneManager::update()
        │           └── Scene::update()
        │
        └── frameTimer.decreaseAccumulator()
  
  └── render()                      // 每帧渲染
        ├── window.clear()
        ├── SceneManager::render()
        │     └── Scene::render()
        └── window.display()
```

---

## 三、核心类说明

### 3.1 Game 类

**文件**: `src/core/Game.h`, `src/core/Game.cpp`

**职责**: 游戏主控制器，管理所有子系统生命周期

**关键方法**:

| 方法            | 说明                     |
| --------------- | ------------------------ |
| `getInstance()` | 单例访问                 |
| `initialize()`  | 初始化窗口、管理器、场景 |
| `run()`         | 运行主循环               |
| `shutdown()`    | 清理所有资源             |

**注意事项**:
- 使用单例模式，全局唯一实例
- 不持有 ResourceManager，通过 `ResourceManager::getInstance()` 访问

### 3.2 FrameTimer 类

**文件**: `src/core/FrameTimer.h`, `src/core/FrameTimer.cpp`

**职责**: 实现 60 FPS 固定时间步长

**核心逻辑**:

```cpp
void FrameTimer::update() {
    float elapsed = m_clock.restart().asSeconds();
    m_accumulator += elapsed;
}

bool FrameTimer::shouldUpdate() const {
    return m_accumulator >= FRAME_TIME;  // 1/60 秒
}

void FrameTimer::decreaseAccumulator() {
    m_accumulator -= FRAME_TIME;
}
```

**为什么使用固定时间步长？**
- 确保游戏逻辑在不同硬件上表现一致
- 避免物理计算因帧率波动产生误差
- 便于帧精确的输入缓冲和连招系统

### 3.3 ResourceManager 类

**文件**: `src/managers/ResourceManager.h`, `src/managers/ResourceManager.cpp`

**职责**: 加载和缓存游戏资源

**设计模式**: 单例 + 缓存

**资源类型**:

| 类型 | 容器                                               | 加载方法            |
| ---- | -------------------------------------------------- | ------------------- |
| 纹理 | `std::unordered_map<std::string, sf::Texture>`     | `loadTexture()`     |
| 字体 | `std::unordered_map<std::string, sf::Font>`        | `loadFont()`        |
| 音效 | `std::unordered_map<std::string, sf::SoundBuffer>` | `loadSoundBuffer()` |

**SFML 3.0 注意事项**:
```cpp
// SFML 3.0 字体加载 API 变化
sf::Font font;
font.openFromFile(path);  // 不是 loadFromFile！

// 纹理加载
sf::Texture texture;
texture.loadFromFile(path);  // 正常
```

### 3.4 SceneManager 类

**文件**: `src/managers/SceneManager.h`, `src/managers/SceneManager.cpp`

**职责**: 管理场景栈，支持场景叠加（如暂停菜单）

**场景栈操作**:

| 方法            | 说明                       | 使用场景     |
| --------------- | -------------------------- | ------------ |
| `pushScene()`   | 压入新场景，暂停当前场景   | 打开暂停菜单 |
| `popScene()`    | 弹出当前场景，恢复下层场景 | 关闭暂停菜单 |
| `changeScene()` | 替换当前场景               | 切换到新场景 |

**场景生命周期**:

```
pushScene(scene)
  ├── 旧场景.onPause()    (如果有)
  └── 新场景.onEnter()

popScene()
  ├── 当前场景.onExit()
  └── 下层场景.onResume()  (如果有)
```

### 3.5 Scene 基类

**文件**: `src/scenes/Scene.h`, `src/scenes/Scene.cpp`

**职责**: 所有场景的抽象基类

**核心接口**:

```cpp
class Scene {
public:
    virtual void onEnter();    // 进入场景
    virtual void onExit();     // 退出场景
    virtual void onPause();    // 暂停场景
    virtual void onResume();   // 恢复场景
    
    virtual void handleEvents(sf::RenderWindow& window) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    virtual bool isFinished() const;
    virtual std::unique_ptr<Scene> getNextScene() const;
};
```

**渲染规则**:
- ❌ 场景中**不要**调用 `window.clear()`
- ❌ 场景中**不要**调用 `window.display()`
- ✅ 由 `Game::render()` 统一调用

---

## 四、主菜单场景实现

### 4.1 MainMenuScene 类

**文件**: `src/scenes/MainMenuScene.h`, `src/scenes/MainMenuScene.cpp`

**菜单项**:

| 索引 | 菜单项     | 动作                                 |
| ---- | ---------- | ------------------------------------ |
| 0    | Start Game | `m_finished = true` (待实现角色选择) |
| 1    | Settings   | 待实现                               |
| 2    | Exit       | `window->close()`                    |

### 4.2 选择状态管理

**成员变量**:

```cpp
int m_selectedIndex;      // 当前选中索引
bool m_mouseHovering;     // 是否鼠标悬停模式
sf::RenderWindow* m_window;  // 窗口指针（用于Exit关闭）
```

**颜色方案**:

| 状态     | 颜色 | RGB             |
| -------- | ---- | --------------- |
| 普通     | 白色 | (255, 255, 255) |
| 键盘选中 | 金色 | (255, 200, 0)   |
| 鼠标悬停 | 浅蓝 | (200, 200, 255) |

### 4.3 事件处理流程

```
handleEvents(window)
  │
  ├── 保存窗口指针: m_window = &window
  │
  ├── 获取鼠标位置: mousePos = Mouse::getPosition(window)
  │
  ├── updateMenuSelection(mousePos)  // 更新悬停状态
  │
  └── while (event = window.pollEvent())
        │
        ├── KeyPressed::Up    → selectPreviousItem()
        ├── KeyPressed::Down  → selectNextItem()
        ├── KeyPressed::Enter → executeCurrentItem()
        ├── KeyPressed::Escape → window.close()
        │
        └── MouseButtonPressed::Left
              └── if (m_mouseHovering) → executeCurrentItem()
```

### 4.4 m_selectedIndex 更新逻辑

**键盘导航**:
```cpp
void selectPreviousItem() {
    m_selectedIndex--;
    if (m_selectedIndex < 0)
        m_selectedIndex = m_menuItems.size() - 1;  // 循环
    m_mouseHovering = false;
}

void selectNextItem() {
    m_selectedIndex++;
    if (m_selectedIndex >= m_menuItems.size())
        m_selectedIndex = 0;  // 循环
    m_mouseHovering = false;
}
```

**鼠标悬停**:
```cpp
void updateMenuSelection(mousePos) {
    for (i = 0; i < m_menuItems.size(); i++) {
        if (bounds.contains(mousePos)) {
            m_selectedIndex = i;
            m_mouseHovering = true;
            // 设置悬停颜色
        }
    }
}
```

---

## 五、配置系统

### 5.1 配置文件

**game_config.json** (`assets/config/game_config.json`):

```json
{
    "window": {
        "width": 1280,
        "height": 720,
        "title": "Samurai Fight",
        "fullscreen": false,
        "vsync": false
    },
    "gameplay": {
        "targetFPS": 60,
        "roundTime": 60,
        "winsNeeded": 2
    },
    "audio": {
        "masterVolume": 1.0,
        "bgmVolume": 0.8,
        "sfxVolume": 1.0
    }
}
```

**keybindings.json** (`assets/config/keybindings.json`):

```json
{
    "player1": {
        "moveLeft": "A", "moveRight": "D",
        "jump": "W", "crouch": "S",
        "attackLight": "J", "attackMedium": "K", "attackHeavy": "L",
        "attackSpecial": "U", "block": "I"
    },
    "player2": {
        "moveLeft": "Left", "moveRight": "Right",
        "jump": "Up", "crouch": "Down",
        "attackLight": "Numpad1", "attackMedium": "Numpad2",
        "attackHeavy": "Numpad3", "attackSpecial": "Numpad4",
        "block": "Numpad5"
    },
    "global": { "pause": "Escape" }
}
```

### 5.2 ConfigManager 使用

```cpp
ConfigManager& config = Game::getInstance().getConfigManager();

int width = config.getWindowWidth();          // 1280
int height = config.getWindowHeight();        // 720
int roundTime = config.getRoundTime();        // 60
float volume = config.getMasterVolume();      // 1.0
```

---

## 六、构建与运行

### 6.1 构建命令

```bash
# 配置项目
cmake -B build -G "MinGW Makefiles"

# 构建
cmake --build build

# 运行
./build/bin/SamuraiFight.exe
```

### 6.2 依赖

| 依赖          | 版本   | 用途             |
| ------------- | ------ | ---------------- |
| SFML          | 3.0.2  | 图形、窗口、音频 |
| nlohmann_json | 3.12.0 | JSON 解析        |

### 6.3 目录结构

```
SamuraiFight/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── Game.h/cpp
│   │   ├── GameLoop.h/cpp
│   │   ├── FrameTimer.h/cpp
│   │   └── Constants.h
│   ├── managers/
│   │   ├── ResourceManager.h/cpp
│   │   ├── SceneManager.h/cpp
│   │   └── ConfigManager.h/cpp
│   ├── scenes/
│   │   ├── Scene.h/cpp
│   │   └── MainMenuScene.h/cpp
│   └── utils/
│       └── JsonLoader.h/cpp
├── assets/
│   └── config/
│       ├── game_config.json
│       └── keybindings.json
└── build/bin/
    └── SamuraiFight.exe
```

---

## 七、已知问题与后续工作

### 7.1 待实现功能

- [ ] 角色选择场景 (CharacterSelectScene)
- [ ] 设置场景 (SettingsScene)
- [ ] 自定义像素字体 (`assets/fonts/pixel_font.ttf`)
- [ ] UI 按钮图片资源

### 7.2 当前限制

1. **字体**: 使用系统字体 Arial，需添加像素字体文件
2. **Start Game**: 仅设置 `m_finished = true`，未实现场景切换
3. **Settings**: 未实现

---

## 八、扩展指南

### 8.1 添加新场景

1. 继承 `Scene` 基类:

```cpp
class MyScene : public Scene {
public:
    void onEnter() override;
    void onExit() override;
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
};
```

2. 在 `Game::initialize()` 中压入场景:

```cpp
auto myScene = std::make_unique<MyScene>();
m_sceneManager->pushScene(std::move(myScene));
```

### 8.2 添加新资源

```cpp
// 加载纹理
ResourceManager::getInstance().loadTexture("player", "assets/characters/player.png");

// 使用纹理
const sf::Texture& tex = ResourceManager::getInstance().getTexture("player");
sf::Sprite sprite(tex);
```

---

**文档维护者**: lllhhh
**最后更新**: 2026年6月18日
