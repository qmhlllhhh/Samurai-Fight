#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

namespace SamuraiFight {

/**
 * @file HealthBar.h
 * @brief 血条UI组件
 */

/**
 * @class HealthBar
 * @brief 显示角色生命值的UI组件
 */
class HealthBar {
public:
    /**
     * @brief 构造函数
     *
     * @param position 位置
     * @param size 尺寸
     * @param isPlayer1 是否是玩家1（影响朝向）
     */
    HealthBar(const sf::Vector2f& position, const sf::Vector2f& size, bool isPlayer1 = true);

    /**
     * @brief 析构函数
     */
    ~HealthBar();

    /**
     * @brief 更新血条显示
     *
     * @param currentHp 当前生命值
     * @param maxHp 最大生命值
     */
    void update(float currentHp, float maxHp);

    /**
     * @brief 渲染血条
     *
     * @param window 游戏窗口
     */
    void render(sf::RenderWindow& window);

    /**
     * @brief 设置位置
     *
     * @param position 新位置
     */
    void setPosition(const sf::Vector2f& position);

    /**
     * @brief 设置尺寸
     *
     * @param size 新尺寸
     */
    void setSize(const sf::Vector2f& size);

private:
    sf::Vector2f m_position;    ///< 位置
    sf::Vector2f m_size;        ///< 尺寸
    bool m_isPlayer1;           ///< 是否是玩家1

    float m_currentPercent;     ///< 当前生命值百分比

    std::unique_ptr<sf::RectangleShape> m_background;  ///< 背景条
    std::unique_ptr<sf::RectangleShape> m_fill;        ///< 填充条
    std::unique_ptr<sf::RectangleShape> m_border;      ///< 边框
};

} // namespace SamuraiFight
