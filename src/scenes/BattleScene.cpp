#include "BattleScene.h"
#include "../core/Constants.h"
#include "../entities/CharacterFactory.h"
#include "../managers/ResourceManager.h"
#include "PauseScene.h"
#include <iostream>

namespace SamuraiFight {

BattleScene::BattleScene()
    : m_backgroundId{"grass3", "forest", "house", "grass1", "grass2"}, m_window(nullptr), m_inputManager(nullptr), m_showDebug(true) {
}

BattleScene::~BattleScene() {
}

void BattleScene::onEnter() {
    std::cout << "BattleScene: Entered" << std::endl;

    // 初始化输入管理器
    m_inputManager = std::make_unique<InputManager>();
    m_inputManager->initialize();

    // 初始化角色
    initializeCharacters();

    // 初始化背景
    initializeBackground();

    // 初始化调试文本
    try {
        const sf::Font &font = ResourceManager::getInstance().getDefaultFont();
        m_debugText = std::make_unique<sf::Text>(font);
        m_debugText->setCharacterSize(16);
        m_debugText->setFillColor(sf::Color::White);
        m_debugText->setPosition(sf::Vector2f(10.0f, 10.0f));
    } catch (const std::exception &e) {
        std::cerr << "BattleScene: Error initializing debug text - " << e.what() << std::endl;
    }

    std::cout << "BattleScene: Initialization complete" << std::endl;
}

void BattleScene::onExit() {
    std::cout << "BattleScene: Exited" << std::endl;

    // 清理角色
    m_characters[0].reset();
    m_characters[1].reset();

    // 清理输入管理器
    m_inputManager.reset();
}

void BattleScene::initializeCharacters() {
    // 加载角色数据
    CharacterFactory &factory = CharacterFactory::getInstance();

    // 创建玩家1角色
    m_characters[0] = factory.createCharacter("musashi", 0);
    if (m_characters[0]) {
        std::cout << "BattleScene: Created player 1 character" << std::endl;
    } else {
        std::cerr << "BattleScene: Failed to create player 1 character" << std::endl;
    }

    // 创建玩家2角色
    m_characters[1] = factory.createCharacter("sakura", 1);
    if (m_characters[1]) {
        std::cout << "BattleScene: Created player 2 character" << std::endl;
    } else {
        std::cerr << "BattleScene: Failed to create player 2 character" << std::endl;
    }
}

void BattleScene::initializeBackground() {
    // 随机选择背景
    int bgIndex = randomInt(0, BACKGROUND_COUNT - 1);
    std::string bgId = m_backgroundId[bgIndex];

    // 检查纹理是否存在
    if (ResourceManager::getInstance().hasTexture(bgId)) {
        const sf::Texture &texture = ResourceManager::getInstance().getTexture(bgId);
        m_background = std::make_unique<sf::Sprite>(texture);
        m_background->setPosition({0, 0});
        m_background->setScale({static_cast<float>(WINDOW_WIDTH) / texture.getSize().x,
                                static_cast<float>(WINDOW_HEIGHT) / texture.getSize().y});
    } else {
        std::cerr << "BattleScene: Background texture '" << bgId << "' not found" << std::endl;
        m_background = nullptr;
    }
}

void BattleScene::handleEvents(sf::RenderWindow &window) {
    // 保存窗口指针
    m_window = &window;

    // 处理窗口事件
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        // 窗口关闭
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        // 键盘按键
        if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyPressed->code) {
            case sf::Keyboard::Key::Escape:
                // 暂停游戏（压入暂停场景）
                m_popSceneCount = 0;
                m_nextScene = std::make_unique<PauseScene>();
                break;

            case sf::Keyboard::Key::F1:
                // 切换调试信息显示
                m_showDebug = !m_showDebug;
                break;

            default:
                break;
            }
        }
    }
}

void BattleScene::update(float deltaTime) {
    // 更新输入管理器
    if (m_inputManager) {
        m_inputManager->update(*m_window);

        // 获取玩家输入并传递给角色
        for (int i = 0; i < 2; ++i) {
            if (m_characters[i]) {
                InputState input = m_inputManager->getPlayerInput(i);
                m_characters[i]->handleInput(
                    input.moveLeft,
                    input.moveRight,
                    input.jump,
                    input.crouch,
                    input.attackLight,
                    input.attackMedium,
                    input.attackHeavy);
            }
        }
    }

    // 更新角色
    for (int i = 0; i < 2; ++i) {
        if (m_characters[i]) {
            m_characters[i]->update(deltaTime);
        }
    }

    // 更新调试文本
    if (m_showDebug && m_characters[0]) {
        InputState input = m_inputManager->getPlayerInput(0);

        std::string debugStr = "Player 1:\n";
        debugStr += "  Position: (" + std::to_string(static_cast<int>(m_characters[0]->getPosition().x)) +
                    ", " + std::to_string(static_cast<int>(m_characters[0]->getPosition().y)) + ")\n";
        debugStr += "  Velocity: (" + std::to_string(static_cast<int>(m_characters[0]->getVelocity().x)) +
                    ", " + std::to_string(static_cast<int>(m_characters[0]->getVelocity().y)) + ")\n";
        debugStr += "  State: " + std::to_string(static_cast<int>(m_characters[0]->getCurrentStateType())) + "\n";
        debugStr += "  OnGround: " + std::string(m_characters[0]->isOnGround() ? "Yes" : "No") + "\n";
        debugStr += "  Input: L=" + std::string(input.moveLeft ? "1" : "0") +
                    " R=" + std::string(input.moveRight ? "1" : "0") +
                    " J=" + std::string(input.jump ? "1" : "0") + "\n";
        debugStr += "\nControls:\n";
        debugStr += "  WASD - Move/Jump/Crouch\n";
        debugStr += "  F1 - Toggle debug info\n";
        debugStr += "  ESC - Pause";

        m_debugText->setString(debugStr);
    }
}

void BattleScene::render(sf::RenderWindow &window) {
    // 绘制背景
    if (m_background) {
        window.draw(*m_background);
    } else {
        // 如果没有背景，使用默认颜色
        window.clear(sf::Color(50, 50, 80));
    }

    // 绘制地面（调试用）
    sf::RectangleShape ground(sf::Vector2f(WINDOW_WIDTH, 5.0f));
    ground.setPosition(sf::Vector2f(0.0f, GROUND_LEVEL));
    ground.setFillColor(sf::Color(100, 100, 100));
    window.draw(ground);

    // 绘制角色
    for (int i = 0; i < 2; ++i) {
        if (m_characters[i]) {
            m_characters[i]->render(window);
        }
    }

    // 绘制调试信息
    if (m_showDebug && m_debugText) {
        window.draw(*m_debugText);
    }
}

} // namespace SamuraiFight