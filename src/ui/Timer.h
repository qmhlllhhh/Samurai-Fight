#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

namespace SamuraiFight {

/**
 * @file Timer.h
 * @brief 计时器UI组件
 */

/**
 * @class Timer
 * @brief 显示回合计时（秒数）的UI组件
 *
 * 剩余 ≤10 秒时文字变红警告。position 为水平中心点（自动居中）。
 */
class Timer {
public:
    /**
     * @brief 构造函数
     *
     * @param position 位置（水平中心 + 顶 Y）
     * @param characterSize 字号，默认 48
     */
    Timer(const sf::Vector2f& position, unsigned int characterSize = 48);

    /**
     * @brief 析构函数
     */
    ~Timer();

    /**
     * @brief 更新计时显示
     *
     * @param secondsRemaining 剩余秒数（<0 视为 0）
     */
    void update(int secondsRemaining);

    /**
     * @brief 渲染计时器
     *
     * @param window 游戏窗口
     */
    void render(sf::RenderWindow& window);

    /**
     * @brief 设置位置（水平中心 + 顶 Y）
     *
     * @param position 新位置
     */
    void setPosition(const sf::Vector2f& position);

private:
    std::unique_ptr<sf::Text> m_text;  ///< 计时文本
    sf::Color m_normalColor;           ///< 正常颜色（白）
    sf::Color m_warningColor;          ///< 警告颜色（红，≤10 秒）
    sf::Vector2f m_position;           ///< 位置（水平中心 + 顶 Y）
};

} // namespace SamuraiFight
