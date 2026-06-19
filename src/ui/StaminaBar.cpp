#include "StaminaBar.h"
#include <algorithm>

namespace SamuraiFight {

StaminaBar::StaminaBar(const sf::Vector2f& position, const sf::Vector2f& size, bool isPlayer1)
    : m_position(position)
    , m_size(size)
    , m_isPlayer1(isPlayer1)
    , m_currentPercent(1.0f) {

    // 创建背景条
    m_background = std::make_unique<sf::RectangleShape>(m_size);
    m_background->setPosition(m_position);
    m_background->setFillColor(sf::Color(40, 40, 40));  // 深灰色背景

    // 创建填充条
    m_fill = std::make_unique<sf::RectangleShape>(m_size);
    m_fill->setPosition(m_position);
    m_fill->setFillColor(sf::Color(50, 150, 255));  // 蓝色体力值

    // 创建边框
    m_border = std::make_unique<sf::RectangleShape>(m_size);
    m_border->setPosition(m_position);
    m_border->setFillColor(sf::Color::Transparent);
    m_border->setOutlineColor(sf::Color::White);
    m_border->setOutlineThickness(1.0f);
}

StaminaBar::~StaminaBar() {
}

void StaminaBar::update(float currentStamina, float maxStamina, float threshold) {
    (void)threshold;  // 不再使用threshold参数

    if (maxStamina <= 0.0f) {
        m_currentPercent = 0.0f;
    } else {
        m_currentPercent = std::clamp(currentStamina / maxStamina, 0.0f, 1.0f);
    }

    // 更新填充条宽度
    sf::Vector2f fillSize = m_size;
    fillSize.x = m_size.x * m_currentPercent;
    m_fill->setSize(fillSize);

    // 根据玩家朝向调整填充方向
    if (m_isPlayer1) {
        // 玩家1：从左到右填充
        m_fill->setPosition(m_position);
    } else {
        // 玩家2：从右到左填充
        m_fill->setPosition(sf::Vector2f(
            m_position.x + m_size.x * (1.0f - m_currentPercent),
            m_position.y
        ));
    }

    // 根据体力值百分比改变颜色
    // 小于20%：红色，20%-50%：橙色，50%-100%：蓝色
    if (m_currentPercent < 0.2f) {
        m_fill->setFillColor(sf::Color(255, 50, 50));  // 红色（<20%）
    } else if (m_currentPercent < 0.5f) {
        m_fill->setFillColor(sf::Color(255, 150, 50));  // 橙色（20%-50%）
    } else {
        m_fill->setFillColor(sf::Color(50, 150, 255));  // 蓝色（50%-100%）
    }
}

void StaminaBar::render(sf::RenderWindow& window) {
    // 先绘制背景
    window.draw(*m_background);
    // 再绘制填充
    window.draw(*m_fill);
    // 最后绘制边框
    window.draw(*m_border);
}

void StaminaBar::setPosition(const sf::Vector2f& position) {
    m_position = position;
    m_background->setPosition(m_position);
    m_border->setPosition(m_position);
}

void StaminaBar::setSize(const sf::Vector2f& size) {
    m_size = size;
    m_background->setSize(m_size);
    m_border->setSize(m_size);
}

} // namespace SamuraiFight
