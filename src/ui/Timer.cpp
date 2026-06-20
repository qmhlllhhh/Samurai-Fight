#include "Timer.h"
#include "../managers/ResourceManager.h"
#include <string>

namespace SamuraiFight {

Timer::Timer(const sf::Vector2f& position, unsigned int characterSize)
    : m_normalColor(sf::Color::White)
    , m_warningColor(sf::Color::Red)
    , m_position(position) {
    const sf::Font& font = ResourceManager::getInstance().getDefaultFont();
    m_text = std::make_unique<sf::Text>(font);
    m_text->setCharacterSize(characterSize);
    m_text->setFillColor(m_normalColor);
    m_text->setString("0");
}

Timer::~Timer() {
}

void Timer::update(int secondsRemaining) {
    if (secondsRemaining < 0) secondsRemaining = 0;

    m_text->setString(std::to_string(secondsRemaining));
    // 剩余 ≤10 秒红色警告
    m_text->setFillColor(secondsRemaining <= 10 ? m_warningColor : m_normalColor);

    // 水平居中（position.x 为中心）
    sf::FloatRect bounds = m_text->getLocalBounds();
    m_text->setPosition(sf::Vector2f(m_position.x - bounds.size.x / 2.0f, m_position.y));
}

void Timer::render(sf::RenderWindow& window) {
    if (m_text) {
        window.draw(*m_text);
    }
}

void Timer::setPosition(const sf::Vector2f& position) {
    m_position = position;
    if (m_text) {
        sf::FloatRect bounds = m_text->getLocalBounds();
        m_text->setPosition(sf::Vector2f(position.x - bounds.size.x / 2.0f, position.y));
    }
}

} // namespace SamuraiFight
