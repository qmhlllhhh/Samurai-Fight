#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace SamuraiFight {

/**
 * @file ScoreDisplay.h
 * @brief 比分显示UI组件
 */

/**
 * @class ScoreDisplay
 * @brief 显示比分（X - Y）+ 胜场圆点标记
 *
 * 比分文本居中，两侧各 winsNeeded 个圆点；已赢得的局圆点为金色实心，
 * 未赢得为灰色。position 为比分文本的水平中心点。
 */
class ScoreDisplay {
public:
    /**
     * @brief 构造函数
     *
     * @param position 位置（比分文本水平中心 + 顶 Y）
     * @param winsNeeded 获胜所需局数（决定每侧圆点数）
     */
    ScoreDisplay(const sf::Vector2f& position, int winsNeeded);

    /**
     * @brief 析构函数
     */
    ~ScoreDisplay();

    /**
     * @brief 更新比分与胜场标记
     *
     * @param roundsWonP1 玩家1胜场
     * @param roundsWonP2 玩家2胜场
     */
    void update(int roundsWonP1, int roundsWonP2);

    /**
     * @brief 渲染
     *
     * @param window 游戏窗口
     */
    void render(sf::RenderWindow& window);

    /**
     * @brief 设置位置（比分文本水平中心 + 顶 Y）
     *
     * @param position 新位置
     */
    void setPosition(const sf::Vector2f& position);

private:
    std::unique_ptr<sf::Text> m_scoreText;                       ///< 比分文本（X - Y）
    std::vector<std::unique_ptr<sf::CircleShape>> m_p1Marks;     ///< P1 胜场圆点
    std::vector<std::unique_ptr<sf::CircleShape>> m_p2Marks;     ///< P2 胜场圆点
    int m_winsNeeded;                                            ///< 获胜所需局数
    sf::Vector2f m_position;                                     ///< 比分文本水平中心 + 顶 Y
};

} // namespace SamuraiFight
