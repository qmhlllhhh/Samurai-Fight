#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "../managers/ResourceManager.h"
#include "../managers/SceneManager.h"
#include "../managers/ConfigManager.h"
#include "FrameTimer.h"
#include "Constants.h"

namespace SamuraiFight {

/**
 * @file Game.h
 * @brief 游戏主类
 */

/**
 * @class Game
 * @brief 游戏核心管理类（单例）
 *
 * 负责管理：
 * - 游戏窗口
 * - 资源管理器
 * - 场景管理器
 * - 配置管理器
 * - 帧计时器
 * - 游戏主循环
 */
class Game {
public:
    /**
     * @brief 获取单例实例
     *
     * @return Game& 单例实例引用
     */
    static Game& getInstance();

    /**
     * @brief 初始化游戏
     *
     * 创建窗口、加载配置、初始化管理器
     *
     * @return true 初始化成功
     * @return false 初始化失败
     */
    bool initialize();

    /**
     * @brief 运行游戏主循环
     */
    void run();

    /**
     * @brief 关闭游戏
     */
    void shutdown();

    /**
     * @brief 获取游戏窗口
     *
     * @return sf::RenderWindow& 窗口引用
     */
    sf::RenderWindow& getWindow();

    /**
     * @brief 获取资源管理器
     *
     * @return ResourceManager& 资源管理器引用
     */
    ResourceManager& getResourceManager();

    /**
     * @brief 获取场景管理器
     *
     * @return SceneManager& 场景管理器引用
     */
    SceneManager& getSceneManager();

    /**
     * @brief 获取配置管理器
     *
     * @return ConfigManager& 配置管理器引用
     */
    ConfigManager& getConfigManager();

    /**
     * @brief 检查游戏是否正在运行
     *
     * @return true 正在运行
     * @return false 已停止
     */
    bool isRunning() const;

    /**
     * @brief 停止游戏循环
     */
    void stop();

private:
    /**
     * @brief 私有构造函数（单例模式）
     */
    Game();

    /**
     * @brief 私有析构函数
     */
    ~Game();

    // 禁用拷贝和移动
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

    /**
     * @brief 创建游戏窗口
     *
     * @return true 创建成功
     * @return false 创建失败
     */
    bool createWindow();

    /**
     * @brief 处理窗口事件
     */
    void handleEvents();

    /**
     * @brief 更新游戏逻辑
     *
     * @param deltaTime 固定时间步长
     */
    void update(float deltaTime);

    /**
     * @brief 渲染游戏画面
     */
    void render();

    std::unique_ptr<sf::RenderWindow> m_window;         ///< 游戏窗口
    std::unique_ptr<SceneManager> m_sceneManager;       ///< 场景管理器
    std::unique_ptr<ConfigManager> m_configManager;     ///< 配置管理器
    std::unique_ptr<FrameTimer> m_frameTimer;           ///< 帧计时器

    bool m_initialized;                                 ///< 是否已初始化
    bool m_running;                                     ///< 是否正在运行
};

} // namespace SamuraiFight