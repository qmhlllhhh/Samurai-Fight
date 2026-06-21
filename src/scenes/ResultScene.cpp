#include "ResultScene.h"
#include "../core/Constants.h"
#include "../managers/ResourceManager.h"
#include "../managers/AudioManager.h"
#include "BattleScene.h"
#include "CharacterSelectScene.h"
#include "MainMenuScene.h"
#include <iostream>
#include <string>

namespace SamuraiFight {

ResultScene::ResultScene(int winner, int roundsWonP1, int roundsWonP2,
                         const std::string& p1CharId, const std::string& p2CharId)
    : m_selectedIndex(0)
    , m_normalColor(sf::Color::White)
    , m_selectedColor(sf::Color(255, 200, 0))
    , m_titleY(120.0f)
    , m_scoreY(230.0f)
    , m_menuStartY(350.0f)
    , m_menuSpacing(70.0f)
    , m_mouseHovering(false)
    , m_winner(winner)
    , m_p1CharId(p1CharId)
    , m_p2CharId(p2CharId) {
    m_roundsWon[0] = roundsWonP1;
    m_roundsWon[1] = roundsWonP2;
}

ResultScene::~ResultScene() {
}

void ResultScene::onEnter() {
    std::cout << "ResultScene: Entered (winner=" << m_winner << ", score "
              << m_roundsWon[0] << "-" << m_roundsWon[1] << ")" << std::endl;

    // 胜利音效（资源缺失时容错返回 false）
    AudioManager::getInstance().playSound("victory");

    try {
        const sf::Font& boldFont = ResourceManager::getInstance().getFont("defaultBold");

        // 胜者标题
        m_titleText = std::make_unique<sf::Text>(boldFont);
        if (m_winner == 0 || m_winner == 1) {
            m_titleText->setString("Player " + std::to_string(m_winner + 1) + " Wins!");
            m_titleText->setFillColor(m_winner == 0 ? sf::Color(100, 200, 255) : sf::Color(255, 150, 150));
        } else {
            m_titleText->setString("Draw");
            m_titleText->setFillColor(sf::Color::White);
        }
        m_titleText->setCharacterSize(64);
        sf::FloatRect tb = m_titleText->getLocalBounds();
        m_titleText->setPosition(sf::Vector2f((WINDOW_WIDTH - tb.size.x) / 2.0f, m_titleY));

        // 比分
        const sf::Font& font = ResourceManager::getInstance().getDefaultFont();
        m_scoreText = std::make_unique<sf::Text>(font);
        m_scoreText->setString(std::to_string(m_roundsWon[0]) + "  -  " + std::to_string(m_roundsWon[1]));
        m_scoreText->setCharacterSize(48);
        m_scoreText->setFillColor(sf::Color::Yellow);
        sf::FloatRect sb = m_scoreText->getLocalBounds();
        m_scoreText->setPosition(sf::Vector2f((WINDOW_WIDTH - sb.size.x) / 2.0f, m_scoreY));

        initMenuItems();
    } catch (const std::exception& e) {
        std::cerr << "ResultScene: Error initializing - " << e.what() << std::endl;
    }
}

void ResultScene::onExit() {
    std::cout << "ResultScene: Exited" << std::endl;
}

void ResultScene::initMenuItems() {
    const sf::Font& font = ResourceManager::getInstance().getDefaultFont();
    m_menuItems.clear();

    std::vector<std::string> items = {"Rematch", "Character Select", "Main Menu"};
    for (size_t i = 0; i < items.size(); ++i) {
        auto text = std::make_unique<sf::Text>(font);
        text->setString(items[i]);
        text->setCharacterSize(36);
        text->setFillColor(m_normalColor);
        sf::FloatRect bounds = text->getLocalBounds();
        text->setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f,
                                       m_menuStartY + i * m_menuSpacing));
        m_menuItems.push_back(std::move(text));
    }

    m_selectedIndex = 0;
    updateMenuSelection(sf::Vector2f(-1, -1));
}

void ResultScene::handleEvents(sf::RenderWindow& window) {
    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
    updateMenuSelection(mousePos);

    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        if (const auto* k = event->getIf<sf::Event::KeyPressed>()) {
            switch (k->code) {
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
                m_selectedIndex = 2;  // Main Menu
                executeCurrentItem();
                break;
            default:
                break;
            }
        }

        if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mb->button == sf::Mouse::Button::Left && m_mouseHovering) {
                executeCurrentItem();
            }
        }
    }
}

void ResultScene::update(float /*deltaTime*/) {
}

void ResultScene::render(sf::RenderWindow& window) {
    // 全屏背景，遮挡下层场景
    sf::RectangleShape bg(sf::Vector2f(static_cast<float>(WINDOW_WIDTH),
                                       static_cast<float>(WINDOW_HEIGHT)));
    bg.setFillColor(sf::Color(20, 20, 30));
    bg.setPosition(sf::Vector2f(0.0f, 0.0f));
    window.draw(bg);

    if (m_titleText) window.draw(*m_titleText);
    if (m_scoreText) window.draw(*m_scoreText);
    for (const auto& item : m_menuItems) {
        if (item) window.draw(*item);
    }
}

void ResultScene::updateMenuSelection(const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        if (!m_menuItems[i]) continue;
        sf::FloatRect bounds = m_menuItems[i]->getGlobalBounds();
        if (mousePos.x >= 0 && mousePos.y >= 0 && bounds.contains(mousePos)) {
            m_selectedIndex = static_cast<int>(i);
            m_mouseHovering = true;
        }
    }
    // 刷新选中颜色
    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        if (m_menuItems[i]) {
            m_menuItems[i]->setFillColor((static_cast<int>(i) == m_selectedIndex) ? m_selectedColor : m_normalColor);
        }
    }
}

void ResultScene::selectPreviousItem() {
    m_selectedIndex--;
    if (m_selectedIndex < 0) m_selectedIndex = static_cast<int>(m_menuItems.size()) - 1;
    updateMenuSelection(sf::Vector2f(-1, -1));
}

void ResultScene::selectNextItem() {
    m_selectedIndex++;
    if (m_selectedIndex >= static_cast<int>(m_menuItems.size())) m_selectedIndex = 0;
    updateMenuSelection(sf::Vector2f(-1, -1));
}

void ResultScene::executeCurrentItem() {
    if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_menuItems.size())) return;

    AudioManager::getInstance().playSound("select");

    switch (m_selectedIndex) {
    case 0:  // Rematch（同角色重赛）
        std::cout << "ResultScene: Rematch (" << m_p1CharId << " vs " << m_p2CharId << ")" << std::endl;
        m_nextScene = std::make_unique<BattleScene>(m_p1CharId, m_p2CharId);
        m_popSceneCount = 1;
        break;
    case 1:  // Character Select
        std::cout << "ResultScene: Character Select" << std::endl;
        m_nextScene = std::make_unique<CharacterSelectScene>();
        m_popSceneCount = 1;
        break;
    case 2:  // Main Menu
        std::cout << "ResultScene: Main Menu" << std::endl;
        m_nextScene = std::make_unique<MainMenuScene>();
        m_popSceneCount = 1;
        break;
    default:
        break;
    }
}

} // namespace SamuraiFight
