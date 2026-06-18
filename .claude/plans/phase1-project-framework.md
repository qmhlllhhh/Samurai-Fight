# 阶段1：项目框架实现计划

## 目标
完成开发路线图中阶段1的任务，交付一个可运行的主菜单窗口。

## 任务清单
- [x] CMake 项目搭建
- [x] Game 类、GameLoop 实现
- [x] ResourceManager 基础实现
- [x] SceneManager 框架
- [x] 主菜单场景

## 实现步骤

### 步骤1：创建项目基础结构

创建以下目录结构：
```
src/
├── main.cpp
├── core/
│   ├── Game.h / Game.cpp
│   ├── GameLoop.h / GameLoop.cpp
│   ├── FrameTimer.h / FrameTimer.cpp
│   └── Constants.h
├── managers/
│   ├── ResourceManager.h / ResourceManager.cpp
│   ├── SceneManager.h / SceneManager.cpp
│   └── ConfigManager.h / ConfigManager.cpp
├── scenes/
│   ├── Scene.h / Scene.cpp
│   └── MainMenuScene.h / MainMenuScene.cpp
└── utils/
    └── JsonLoader.h / JsonLoader.cpp
assets/
├── config/
│   ├── game_config.json
│   └── keybindings.json
├── fonts/
└── ui/
    └── buttons/
include/  (第三方库头文件)
```

### 步骤2：CMakeLists.txt 配置

配置内容：
- CMake 最低版本 3.16
- C++20 标准
- SFML 3.0 组件：Graphics, Window, System, Audio
- nlohmann/json 头文件库
- 源文件组织
- 输出可执行文件 SamuraiFight

### 步骤3：核心类实现

#### 3.1 Constants.h
定义游戏常量：
- TARGET_FPS = 60
- FRAME_TIME = 1.0f / 60.0f
- WINDOW_WIDTH = 1280
- WINDOW_HEIGHT = 720
- MAX_HEALTH = 100
- MAX_STAMINA = 100

#### 3.2 FrameTimer
帧计时器，实现60 FPS固定时间步长：
```cpp
class FrameTimer {
public:
    void update();           // 更新累积时间
    bool shouldUpdate();     // 是否应该执行逻辑更新
    void decreaseAccumulator(); // 消耗一帧时间
private:
    float m_accumulator = 0.0f;
    sf::Clock m_clock;
};
```

#### 3.3 GameLoop
游戏循环控制：
```cpp
class GameLoop {
public:
    void run();
    void handleEvents();
    void update(float deltaTime);
    void render();
};
```

#### 3.4 Game (单例)
游戏主类，管理所有子系统：
```cpp
class Game {
public:
    static Game& getInstance();
    void initialize();
    void run();
    void shutdown();
    
    ResourceManager& getResourceManager();
    SceneManager& getSceneManager();
    ConfigManager& getConfigManager();
    sf::RenderWindow& getWindow();
    
private:
    Game() = default;
    std::unique_ptr<ResourceManager> m_resourceManager;
    std::unique_ptr<SceneManager> m_sceneManager;
    std::unique_ptr<ConfigManager> m_configManager;
    std::unique_ptr<sf::RenderWindow> m_window;
    std::unique_ptr<FrameTimer> m_frameTimer;
};
```

### 步骤4：管理器实现

#### 4.1 ResourceManager
资源管理器，加载和缓存资源：
```cpp
class ResourceManager {
public:
    bool loadTexture(const std::string& id, const std::string& path);
    const sf::Texture& getTexture(const std::string& id) const;
    
    bool loadFont(const std::string& id, const std::string& path);
    const sf::Font& getFont(const std::string& id) const;
    
    void clear();
    
private:
    std::unordered_map<std::string, sf::Texture> m_textures;
    std::unordered_map<std::string, sf::Font> m_fonts;
};
```

#### 4.2 SceneManager
场景管理器，管理场景栈：
```cpp
class SceneManager {
public:
    void pushScene(std::unique_ptr<Scene> scene);
    void popScene();
    void changeScene(std::unique_ptr<Scene> scene);
    
    Scene* getCurrentScene() const;
    
    void handleEvents(const sf::Event& event);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
private:
    std::vector<std::unique_ptr<Scene>> m_sceneStack;
};
```

#### 4.3 ConfigManager
配置管理器，加载JSON配置：
```cpp
class ConfigManager {
public:
    bool loadGameConfig(const std::string& path);
    bool loadKeyBindings(const std::string& path);
    
    const nlohmann::json& getGameConfig() const;
    const nlohmann::json& getKeyBindings() const;
    
private:
    nlohmann::json m_gameConfig;
    nlohmann::json m_keyBindings;
};
```

### 步骤5：场景系统实现

#### 5.1 Scene (基类)
场景基类：
```cpp
class Scene {
public:
    virtual ~Scene() = default;
    
    virtual void onEnter() {}   // 进入场景时调用
    virtual void onExit() {}    // 退出场景时调用
    
    virtual void handleEvents(const sf::Event& event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};
```

#### 5.2 MainMenuScene
主菜单场景：
- 显示游戏标题
- 显示菜单选项：开始游戏、设置、退出
- 处理键盘/鼠标输入
- 场景切换逻辑

### 步骤6：工具类实现

#### 6.1 JsonLoader
JSON文件加载工具：
```cpp
namespace JsonLoader {
    bool loadFromFile(const std::string& path, nlohmann::json& json);
    bool saveToFile(const std::string& path, const nlohmann::json& json);
}
```

### 步骤7：配置文件

#### 7.1 game_config.json
```json
{
    "window": {
        "width": 1280,
        "height": 720,
        "title": "Samurai Fight",
        "fullscreen": false,
        "vsync": true
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

#### 7.2 keybindings.json
```json
{
    "player1": {
        "moveLeft": "A",
        "moveRight": "D",
        "jump": "W",
        "crouch": "S",
        "attackLight": "J",
        "attackMedium": "K",
        "attackHeavy": "L",
        "attackSpecial": "U",
        "block": "I"
    },
    "player2": {
        "moveLeft": "Left",
        "moveRight": "Right",
        "jump": "Up",
        "crouch": "Down",
        "attackLight": "Numpad1",
        "attackMedium": "Numpad2",
        "attackHeavy": "Numpad3",
        "attackSpecial": "Numpad4",
        "block": "Numpad5"
    },
    "global": {
        "pause": "Escape"
    }
}
```

### 步骤8：主程序入口

main.cpp：
```cpp
#include "core/Game.h"

int main() {
    auto& game = Game::getInstance();
    game.initialize();
    game.run();
    game.shutdown();
    return 0;
}
```

## 文件创建顺序

1. CMakeLists.txt
2. src/core/Constants.h
3. src/utils/JsonLoader.h / JsonLoader.cpp
4. src/core/FrameTimer.h / FrameTimer.cpp
5. src/core/GameLoop.h / GameLoop.cpp
6. src/managers/ConfigManager.h / ConfigManager.cpp
7. src/managers/ResourceManager.h / ResourceManager.cpp
8. src/scenes/Scene.h / Scene.cpp
9. src/managers/SceneManager.h / SceneManager.cpp
10. src/scenes/MainMenuScene.h / MainMenuScene.cpp
11. src/core/Game.h / Game.cpp
12. src/main.cpp
13. assets/config/game_config.json
14. assets/config/keybindings.json

## 依赖说明

### SFML 3.0
- 需要安装 SFML 3.0
- 链接组件：sfml-graphics, sfml-window, sfml-system, sfml-audio

### nlohmann/json
- 使用单头文件 json.hpp
- 放置在 include/ 目录

## 预期结果

完成后：
- 项目可以通过 CMake 配置和构建
- 运行程序显示 1280x720 窗口
- 显示主菜单界面
- 可以通过菜单选项进行交互
- 按 ESC 或点击退出可关闭程序
