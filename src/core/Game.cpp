#include "Game.h"
#include "../scenes/MainMenuScene.h"

#include <iostream>

namespace SamuraiFight {

Game &Game::getInstance() {
    static Game instance;
    return instance;
}

Game::Game()
    : m_window(nullptr), m_sceneManager(nullptr), m_configManager(nullptr), m_frameTimer(nullptr), m_initialized(false), m_running(false) {
    std::cout << "Game: Instance created" << std::endl;
}

Game::~Game() {
    shutdown();
    std::cout << "Game: Instance destroyed" << std::endl;
}

bool Game::initialize() {
    if (m_initialized) {
        std::cerr << "Game: Already initialized" << std::endl;
        return true;
    }

    std::cout << "Game: Initializing..." << std::endl;

    // 创建配置管理器
    m_configManager = std::make_unique<ConfigManager>();
    if (!m_configManager->initialize()) {
        std::cerr << "Game: Warning - ConfigManager initialization had issues" << std::endl;
    }

    // 初始化资源管理器（使用单例）
    if (!ResourceManager::getInstance().initialize()) {
        std::cerr << "Game: Warning - ResourceManager initialization had issues" << std::endl;
    }

    // 创建场景管理器
    m_sceneManager = std::make_unique<SceneManager>();

    // 创建帧计时器
    m_frameTimer = std::make_unique<FrameTimer>();

    // 初始化角色工厂（单例）
    if (!CharacterFactory::getInstance().initialize()) {
        std::cerr << "Game: Warning - CharacterFactory initialization had issues" << std::endl;
    }

    // 创建游戏窗口
    if (!createWindow()) {
        std::cerr << "Game: Failed to create window" << std::endl;
        return false;
    }

    // 压入主菜单场景
    auto mainMenu = std::make_unique<MainMenuScene>();
    m_sceneManager->pushScene(std::move(mainMenu));

    m_initialized = true;
    std::cout << "Game: Initialization complete" << std::endl;
    return true;
}

void Game::run() {
    if (!m_initialized) {
        std::cerr << "Game: Cannot run - not initialized" << std::endl;
        return;
    }

    std::cout << "Game: Starting main loop" << std::endl;
    m_running = true;

    // 主循环
    while (m_running && m_window && m_window->isOpen()) {
        // 更新帧计时器
        m_frameTimer->update();

        // 固定时间步长逻辑更新
        while (m_frameTimer->shouldUpdate()) {
            // 处理事件
            handleEvents();

            // 更新游戏逻辑
            update(FRAME_TIME);

            // 消耗累积时间
            m_frameTimer->decreaseAccumulator();
        }

        // 渲染
        render();
    }

    std::cout << "Game: Main loop ended" << std::endl;
}

void Game::shutdown() {
    if (!m_initialized) {
        return;
    }

    std::cout << "Game: Shutting down..." << std::endl;

    m_running = false;

    // 清理场景管理器
    if (m_sceneManager) {
        m_sceneManager->clearScenes();
    }

    // 关闭窗口
    if (m_window && m_window->isOpen()) {
        m_window->close();
    }

    // 清理管理器
    m_sceneManager.reset();
    m_configManager.reset();
    m_frameTimer.reset();
    m_window.reset();

    // 清理资源管理器单例
    ResourceManager::getInstance().clear();

    m_initialized = false;
    std::cout << "Game: Shutdown complete" << std::endl;
}

sf::RenderWindow &Game::getWindow() {
    if (!m_window) {
        throw std::runtime_error("Game: Window not available");
    }
    return *m_window;
}

ResourceManager &Game::getResourceManager() {
    return ResourceManager::getInstance();
}

SceneManager &Game::getSceneManager() {
    if (!m_sceneManager) {
        throw std::runtime_error("Game: SceneManager not available");
    }
    return *m_sceneManager;
}

ConfigManager &Game::getConfigManager() {
    if (!m_configManager) {
        throw std::runtime_error("Game: ConfigManager not available");
    }
    return *m_configManager;
}

bool Game::isRunning() const {
    return m_running;
}

void Game::stop() {
    m_running = false;
}

bool Game::createWindow() {
    // 从配置获取窗口设置
    int width = m_configManager->getWindowWidth();
    int height = m_configManager->getWindowHeight();
    std::string title = m_configManager->getWindowTitle();
    bool fullscreen = m_configManager->isFullscreen();
    bool vsync = m_configManager->isVSyncEnabled();

    // 创建窗口
    sf::VideoMode videoMode(sf::Vector2u(width, height));

    if (fullscreen) {
        m_window = std::make_unique<sf::RenderWindow>(
            videoMode,
            sf::String::fromUtf8(title.begin(), title.end()),
            sf::Style::Resize | sf::Style::Close);
    } else {
        m_window = std::make_unique<sf::RenderWindow>(
            videoMode,
            sf::String::fromUtf8(title.begin(), title.end()),
            sf::Style::Titlebar | sf::Style::Close);
    }

    if (!m_window) {
        std::cerr << "Game: Failed to create window" << std::endl;
        return false;
    }

    // 设置VSync
    m_window->setVerticalSyncEnabled(vsync);

    // 设置帧率限制（如果VSync关闭）
    if (!vsync) {
        m_window->setFramerateLimit(TARGET_FPS);
    }

    std::cout << "Game: Window created (" << width << "x" << height << ")" << std::endl;
    return true;
}

void Game::handleEvents() {
    if (!m_window || !m_sceneManager) {
        return;
    }

    // 让场景管理器处理事件
    m_sceneManager->handleEvents(*m_window);

    // 处理窗口关闭事件
    while (const std::optional<sf::Event> event = m_window->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_running = false;
            m_window->close();
            return;
        }
    }
}

void Game::update(float deltaTime) {
    if (!m_sceneManager) {
        return;
    }

    // 更新场景管理器
    m_sceneManager->update(deltaTime);

    // 如果场景栈为空，停止游戏
    if (m_sceneManager->isEmpty()) {
        m_running = false;
    }
}

void Game::render() {
    if (!m_window || !m_sceneManager) {
        return;
    }

    // 清屏 - 使用深色背景
    m_window->clear(sf::Color(20, 20, 30));

    // 渲染场景
    m_sceneManager->render(*m_window);

    // 显示
    m_window->display();
}

} // namespace SamuraiFight