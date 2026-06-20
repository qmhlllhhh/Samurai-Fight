#include "PauseScene.h"
#include "../core/Constants.h"
#include "../managers/ResourceManager.h"
#include <iostream>

namespace SamuraiFight {
PauseScene::PauseScene() : m_selectedIndex(0), m_normalColor(sf::Color::White), m_selectedColor(sf::Color(255, 200, 0)), // 金色
                           m_titleY(100.0f), m_menuStartY(300.0f), m_menuSpacing(80.0f), m_mouseHovering(false), m_window(nullptr) {}
PauseScene::~PauseScene() {}

void PauseScene::onEnter() {
    std::cout << "PauseScene: Entered" << std::endl;

    // 获取默认字体
    try {
        const sf::Font &font = ResourceManager::getInstance().getFont("defaultBold");

        // 设置暂停文本
        m_pauseText = std::make_unique<sf::Text>(font);
        m_pauseText->setString("Game Paused");
        m_pauseText->setCharacterSize(72);
        m_pauseText->setFillColor(sf::Color::White);

        // 将暂停文本居中
        sf::FloatRect titleBounds = m_pauseText->getLocalBounds();
        m_pauseText->setPosition(sf::Vector2f(
            (WINDOW_WIDTH - titleBounds.size.x) / 2.0f,
            m_titleY));

        // 初始化菜单项
        initMenuItems();

    } catch (const std::exception &e) {
        std::cerr << "PauseScene: Error initializing - " << e.what() << std::endl;
    }
}

void PauseScene::onExit() {
    std::cout << "PauseScene: Exited" << std::endl;
}
void PauseScene::handleEvents(sf::RenderWindow &window) {
    // 保存窗口指针
    m_window = &window;

    // 处理鼠标移动
    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

    // 更新鼠标悬停状态
    updateMenuSelection(mousePos);

    // 处理键盘输入
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        // 窗口关闭
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        // 键盘按键
        if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyPressed->code) {
            case sf::Keyboard::Key::Up:
                m_mouseHovering = false;
                selectPreviousItem();
                break;

            case sf::Keyboard::Key::Down:
                m_mouseHovering = false;
                selectNextItem();
                break;

            case sf::Keyboard::Key::Enter:
                executeCurrentItem();
                break;

            case sf::Keyboard::Key::Escape:
                m_popSceneCount = 1; // 设置需要弹出一个场景（即当前暂停场景）
                break;

            default:
                break;
            }
        }

        // 鼠标点击
        if (const auto *mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                if (m_mouseHovering) {
                    handleMenuClick();
                }
            }
        }
    }
}
void PauseScene::update(float /*deltaTime*/) {
    // 暂停场景不需要更新逻辑
}
void PauseScene::render(sf::RenderWindow &window) {
    // 绘制暂停文本
    if (m_pauseText) {
        window.draw(*m_pauseText);
    }

    // 绘制菜单项
    for (const auto &menuItem : m_menuItems) {
        if (menuItem) {
            window.draw(*menuItem);
        }
    }
}
void PauseScene::updateMenuSelection(const sf::Vector2f &mousePos) {
    bool foundHover = false;

    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        if (!m_menuItems[i])
            continue;

        sf::FloatRect bounds = m_menuItems[i]->getGlobalBounds();

        // 检查鼠标是否悬停在菜单项上
        if (mousePos.x >= 0 && mousePos.y >= 0 && bounds.contains(mousePos)) {
            // 鼠标在这个菜单项上
            m_selectedIndex = static_cast<int>(i);
            m_mouseHovering = true;
            foundHover = true;
            m_menuItems[i]->setFillColor(m_selectedColor);
        } else {
            // 非悬停状态，根据键盘选择设置颜色
            if (static_cast<int>(i) == m_selectedIndex && !m_mouseHovering) {
                m_menuItems[i]->setFillColor(m_selectedColor);
            } else if (static_cast<int>(i) == m_selectedIndex && foundHover) {
                // 如果鼠标悬停在其他项上，之前的选中项恢复普通颜色
                m_menuItems[i]->setFillColor(m_normalColor);
            } else {
                m_menuItems[i]->setFillColor(m_normalColor);
            }
        }
    }

    // 如果鼠标不在任何菜单项上，恢复键盘选择的颜色
    if (!foundHover && mousePos.x >= 0 && mousePos.y >= 0) {
        m_mouseHovering = false;
        for (size_t i = 0; i < m_menuItems.size(); ++i) {
            if (!m_menuItems[i])
                continue;

            if (static_cast<int>(i) == m_selectedIndex) {
                m_menuItems[i]->setFillColor(m_selectedColor);
            } else {
                m_menuItems[i]->setFillColor(m_normalColor);
            }
        }
    }
}
void PauseScene::initMenuItems() {
    const sf::Font &font = ResourceManager::getInstance().getDefaultFont();

    m_menuItems.clear();
    m_menuActions.clear();

    // 创建菜单项
    std::vector<std::string> itemStrings = {
        "Resume",
        "Settings",
        "Main Menu"};

    std::vector<MenuItem> actions = {
        MenuItem::Resume,
        MenuItem::Settings,
        MenuItem::MainMenu};

    for (size_t i = 0; i < itemStrings.size(); ++i) {
        auto text = std::make_unique<sf::Text>(font);
        text->setString(itemStrings[i]);
        text->setCharacterSize(40);
        text->setFillColor(m_normalColor);

        // 将菜单项居中
        sf::FloatRect bounds = text->getLocalBounds();
        text->setPosition(sf::Vector2f(
            (WINDOW_WIDTH - bounds.size.x) / 2.0f,
            m_menuStartY + i * m_menuSpacing));

        m_menuItems.push_back(std::move(text));
        m_menuActions.push_back(actions[i]);
    }

    // 默认选中第一个菜单项
    m_selectedIndex = 0;
    updateMenuSelection(sf::Vector2f(-1, -1)); // 使用键盘选择
}
void PauseScene::handleMenuClick() {
    executeCurrentItem();
}
void PauseScene::selectPreviousItem() {
    m_selectedIndex--;
    if (m_selectedIndex < 0) {
        m_selectedIndex = static_cast<int>(m_menuItems.size()) - 1;
    }
    updateMenuSelection(sf::Vector2f(-1, -1));
}

void PauseScene::selectNextItem() {
    m_selectedIndex++;
    if (m_selectedIndex >= static_cast<int>(m_menuItems.size())) {
        m_selectedIndex = 0;
    }
    updateMenuSelection(sf::Vector2f(-1, -1));
}
void PauseScene::executeCurrentItem() {
    if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_menuActions.size())) {
        return;
    }

    MenuItem action = m_menuActions[m_selectedIndex];

    switch (action) {
    case MenuItem::Resume:
        std::cout << "PauseScene: back to BattleScene" << std::endl;
        // 切换到战斗场景
        m_popSceneCount = 1; // 设置需要弹出一个场景（即当前暂停场景）
        break;

    case MenuItem::Settings:
        std::cout << "PauseScene: Settings selected" << std::endl;
        // TODO: 切换到设置场景
        // m_nextScene = std::make_unique<SettingsScene>();
        // m_popSceneCount = 1;
        std::cout << "PauseScene: SettingsScene not implemented yet" << std::endl;
        break;

    case MenuItem::MainMenu:
        std::cout << "PauseScene: MainMenu selected" << std::endl;
        // 返回主菜单
        m_nextScene = std::make_unique<MainMenuScene>();
        m_popSceneCount = 2;
        break;
    }
}
} // namespace SamuraiFight