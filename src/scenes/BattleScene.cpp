#include "BattleScene.h"
#include "../core/Constants.h"
#include "../managers/ResourceManager.h"
#include "MainMenuScene.h"
#include "PauseScene.h"
#include <iostream>

namespace SamuraiFight {

BattleScene::BattleScene() : m_backgroundId{"forest", "house", "grass1", "grass2", "grass3"}, m_window(nullptr) {
}

BattleScene::~BattleScene() {}

void BattleScene::onEnter() {
    std::cout << "BattleScene: Entered" << std::endl;
    // 初始化战斗界面
    try {
        // SFML 3.0: Sprite 构造需要纹理参数
        // 随机选择背景
        int bgIndex = randomInt(0, BACKGROUND_COUNT - 1);
        std::string bgId = m_backgroundId[bgIndex];

        // 检查纹理是否存在，如果不存在则使用默认背景色
        if (ResourceManager::getInstance().hasTexture(bgId)) {
            const sf::Texture &texture = ResourceManager::getInstance().getTexture(bgId);
            m_background = std::make_unique<sf::Sprite>(texture);
            m_background->setPosition({0, 0});
            m_background->setScale({static_cast<float>(WINDOW_WIDTH) / texture.getSize().x,
                                    static_cast<float>(WINDOW_HEIGHT) / texture.getSize().y});
        } else {
            std::cerr << "BattleScene: Background texture '" << bgId << "' not found, using default color" << std::endl;
            m_background = nullptr;
        }

    } catch (const std::exception &e) {
        std::cerr << "BattleScene: Error initializing - " << e.what() << std::endl;
        m_background = nullptr;
    }
}

void BattleScene::onExit() {
    std::cout << "BattleScene: Exited" << std::endl;
}

void BattleScene::handleEvents(sf::RenderWindow &window) {
    m_window = &window; // 保存窗口指针以便后续使用
    // 处理玩家输入
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        // 窗口关闭
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        // ESC 键暂停
        if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_nextScene = std::make_unique<PauseScene>();
                return;
            }
        }

        // 其他输入处理逻辑（角色移动、攻击等）
        // TODO: 后续实现角色控制
    }
}

void BattleScene::update(float /*deltaTime*/) {
    // 更新战斗逻辑
    // TODO: 后续实现
    // 例如：处理角色移动、攻击、技能冷却等
}

void BattleScene::render(sf::RenderWindow &window) {
    // 绘制战斗背景
    if (m_background) {
        window.draw(*m_background);
    } else {
        // 注意：这里只是临时方案，后续应该有专门的背景渲染
    }

    // TODO: 绘制角色、UI等其他战斗界面元素
}

} // namespace SamuraiFight