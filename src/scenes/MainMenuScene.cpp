#include "MainMenuScene.h"
#include "../core/Constants.h"
#include "../managers/AudioManager.h"
#include "../managers/ResourceManager.h"
#include "CharacterSelectScene.h"

#include <iostream>

namespace SamuraiFight {

MainMenuScene::MainMenuScene()
    : m_selectedIndex(0), m_normalColor(sf::Color::White), m_selectedColor(sf::Color(255, 200, 0)) // 金色
      ,
      m_titleY(100.0f), m_menuStartY(300.0f), m_menuSpacing(80.0f), m_mouseHovering(false), m_window(nullptr) {
}

MainMenuScene::~MainMenuScene() {
}

void MainMenuScene::onEnter() {
    std::cout << "MainMenuScene: Entered" << std::endl;

    // 结束正在播放的音乐
    AudioManager::getInstance().stopMusic(1.0f);
    // 播放背景音乐
    AudioManager::getInstance().playSceneMusic("menu", 2.0f);

    // 获取默认字体
    try {
        const sf::Font &font = ResourceManager::getInstance().getFont("defaultBold");

        // 设置标题
        m_titleText = std::make_unique<sf::Text>(font);
        m_titleText->setString("Samurai Fight");
        m_titleText->setCharacterSize(72);
        m_titleText->setFillColor(sf::Color::White);

        // 将标题居中
        sf::FloatRect titleBounds = m_titleText->getLocalBounds();
        m_titleText->setPosition(sf::Vector2f(
            (WINDOW_WIDTH - titleBounds.size.x) / 2.0f,
            m_titleY));

        // 初始化菜单项
        initMenuItems();

    } catch (const std::exception &e) {
        std::cerr << "MainMenuScene: Error initializing - " << e.what() << std::endl;
    }
}

void MainMenuScene::onExit() {
    std::cout << "MainMenuScene: Exited" << std::endl;
}

void MainMenuScene::initMenuItems() {
    const sf::Font &font = ResourceManager::getInstance().getDefaultFont();

    m_menuItems.clear();
    m_menuActions.clear();

    // 创建菜单项
    std::vector<std::string> itemStrings = {
        "Start Game",
        "Settings",
        "Exit"};

    std::vector<MenuItem> actions = {
        MenuItem::StartGame,
        MenuItem::Settings,
        MenuItem::Exit};

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

void MainMenuScene::handleEvents(sf::RenderWindow &window) {
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
                // 退出游戏
                window.close();
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

void MainMenuScene::update(float /*deltaTime*/) {
    // 主菜单不需要复杂的逻辑更新
    // 可以添加简单的动画效果（如标题闪烁等）
}

void MainMenuScene::render(sf::RenderWindow &window) {
    // 注意：不要在这里调用 window.clear()，由 Game::render() 统一调用

    // 绘制标题
    if (m_titleText) {
        window.draw(*m_titleText);
    }

    // 绘制菜单项
    for (const auto &item : m_menuItems) {
        if (item) {
            window.draw(*item);
        }
    }

    // 显示提示文字
    try {
        const sf::Font &font = ResourceManager::getInstance().getDefaultFont();
        sf::Text hintText(font);
        hintText.setString("Use Arrow Keys or Mouse to navigate, Enter or Click to select");
        hintText.setCharacterSize(20);
        hintText.setFillColor(sf::Color(150, 150, 150));

        sf::FloatRect hintBounds = hintText.getLocalBounds();
        hintText.setPosition(sf::Vector2f(
            (WINDOW_WIDTH - hintBounds.size.x) / 2.0f,
            WINDOW_HEIGHT - 50.0f));

        window.draw(hintText);
    } catch (const std::exception &e) {
        std::cerr << "MainMenuScene: Error rendering hint text - " << e.what() << std::endl;
    }

    // 注意：不要在这里调用 window.display()，由 Game::render() 统一调用
}

void MainMenuScene::updateMenuSelection(const sf::Vector2f &mousePos) {
    bool foundHover = false;

    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        if (!m_menuItems[i])
            continue;

        sf::FloatRect bounds = m_menuItems[i]->getGlobalBounds();

        // 检查鼠标是否悬停在菜单项上
        if (mousePos.x >= 0 && mousePos.y >= 0 && bounds.contains(mousePos)) {
            // 鼠标在这个菜单项上

            // 如果切换菜单项，播放音效
            if (m_selectedIndex != static_cast<int>(i)) {
                AudioManager::getInstance().playSound("cursor");
            }

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

void MainMenuScene::handleMenuClick() {
    executeCurrentItem();
}

void MainMenuScene::selectPreviousItem() {
    AudioManager::getInstance().playSound("cursor");
    m_selectedIndex--;
    if (m_selectedIndex < 0) {
        m_selectedIndex = static_cast<int>(m_menuItems.size()) - 1;
    }
    updateMenuSelection(sf::Vector2f(-1, -1));
}

void MainMenuScene::selectNextItem() {
    AudioManager::getInstance().playSound("cursor");
    m_selectedIndex++;
    if (m_selectedIndex >= static_cast<int>(m_menuItems.size())) {
        m_selectedIndex = 0;
    }
    updateMenuSelection(sf::Vector2f(-1, -1));
}

void MainMenuScene::executeCurrentItem() {
    if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_menuActions.size())) {
        return;
    }

    AudioManager::getInstance().playSound("select");

    MenuItem action = m_menuActions[m_selectedIndex];

    switch (action) {
    case MenuItem::StartGame:
        std::cout << "MainMenuScene: Start Game selected" << std::endl;
        // 切换到角色选择场景
        m_nextScene = std::make_unique<CharacterSelectScene>();
        m_popSceneCount = 1;
        break;

    case MenuItem::Settings:
        std::cout << "MainMenuScene: Settings selected" << std::endl;
        // TODO: 切换到设置场景
        // m_nextScene = std::make_unique<SettingsScene>();
        // m_popSceneCount=1
        std::cout << "MainMenuScene: SettingsScene not implemented yet" << std::endl;
        break;

    case MenuItem::Exit:
        std::cout << "MainMenuScene: Exit selected" << std::endl;
        // 关闭窗口退出游戏
        if (m_window) {
            m_window->close();
        }
        m_popSceneCount = 1;
        break;
    }
}

} // namespace SamuraiFight