#include "ScoreDisplay.h"
#include "../managers/ResourceManager.h"
#include <string>

namespace SamuraiFight {

ScoreDisplay::ScoreDisplay(const sf::Vector2f& position, int winsNeeded)
    : m_winsNeeded(winsNeeded)
    , m_position(position) {
    const sf::Font& font = ResourceManager::getInstance().getDefaultFont();

    m_scoreText = std::make_unique<sf::Text>(font);
    m_scoreText->setCharacterSize(32);
    m_scoreText->setFillColor(sf::Color::Yellow);
    m_scoreText->setString("0 - 0");

    // 每侧 winsNeeded 个圆点
    constexpr float MARK_RADIUS = 7.0f;
    for (int i = 0; i < winsNeeded; ++i) {
        auto m1 = std::make_unique<sf::CircleShape>(MARK_RADIUS);
        auto m2 = std::make_unique<sf::CircleShape>(MARK_RADIUS);
        m1->setFillColor(sf::Color(80, 80, 80));
        m2->setFillColor(sf::Color(80, 80, 80));
        m_p1Marks.push_back(std::move(m1));
        m_p2Marks.push_back(std::move(m2));
    }
}

ScoreDisplay::~ScoreDisplay() {
}

void ScoreDisplay::update(int roundsWonP1, int roundsWonP2) {
    // 比分文本（居中）
    m_scoreText->setString(std::to_string(roundsWonP1) + " - " + std::to_string(roundsWonP2));
    sf::FloatRect bounds = m_scoreText->getLocalBounds();
    float textLeft = m_position.x - bounds.size.x / 2.0f;
    float textRight = m_position.x + bounds.size.x / 2.0f;
    m_scoreText->setPosition(sf::Vector2f(textLeft, m_position.y));

    // 圆点：已赢得金色，未赢得灰色
    const sf::Color wonColor(255, 200, 0);
    const sf::Color lostColor(80, 80, 80);
    for (int i = 0; i < m_winsNeeded; ++i) {
        if (i < static_cast<int>(m_p1Marks.size()) && m_p1Marks[i]) {
            m_p1Marks[i]->setFillColor(i < roundsWonP1 ? wonColor : lostColor);
        }
        if (i < static_cast<int>(m_p2Marks.size()) && m_p2Marks[i]) {
            m_p2Marks[i]->setFillColor(i < roundsWonP2 ? wonColor : lostColor);
        }
    }

    // 圆点位置：P1 在比分左侧、P2 在右侧（圆点排在比分文本旁）
    constexpr float MARK_SPACING = 20.0f;
    const float markY = m_position.y + 12.0f;
    for (int i = 0; i < m_winsNeeded; ++i) {
        // P1：从比分左侧向左排列（靠近比分的代表最近赢得的局）
        if (i < static_cast<int>(m_p1Marks.size()) && m_p1Marks[i]) {
            float x = textLeft - 16.0f - i * MARK_SPACING;
            m_p1Marks[i]->setPosition(sf::Vector2f(x, markY));
        }
        // P2：从比分右侧向右排列
        if (i < static_cast<int>(m_p2Marks.size()) && m_p2Marks[i]) {
            float x = textRight + 6.0f + i * MARK_SPACING;
            m_p2Marks[i]->setPosition(sf::Vector2f(x, markY));
        }
    }
}

void ScoreDisplay::render(sf::RenderWindow& window) {
    if (m_scoreText) {
        window.draw(*m_scoreText);
    }
    for (const auto& m : m_p1Marks) {
        if (m) window.draw(*m);
    }
    for (const auto& m : m_p2Marks) {
        if (m) window.draw(*m);
    }
}

void ScoreDisplay::setPosition(const sf::Vector2f& position) {
    m_position = position;
}

} // namespace SamuraiFight
