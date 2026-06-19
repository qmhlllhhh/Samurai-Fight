#pragma once

#include <SFML/Graphics.hpp>

namespace SamuraiFight {

/**
 * @file Entity.h
 * @brief 实体基类
 */

/**
 * @class Entity
 * @brief 所有游戏实体的基类
 *
 * 提供位置、速度、渲染等基本功能
 */
class Entity {
public:
    /**
     * @brief 构造函数
     */
    Entity();

    /**
     * @brief 虚析构函数
     */
    virtual ~Entity();

    /**
     * @brief 更新实体逻辑
     *
     * @param deltaTime 时间增量
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief 渲染实体
     *
     * @param window 游戏窗口
     */
    virtual void render(sf::RenderWindow& window) = 0;

    /**
     * @brief 获取位置
     *
     * @return sf::Vector2f 位置
     */
    sf::Vector2f getPosition() const;

    /**
     * @brief 设置位置
     *
     * @param position 新位置
     */
    void setPosition(const sf::Vector2f& position);

    /**
     * @brief 移动实体
     *
     * @param offset 移动偏移
     */
    void move(const sf::Vector2f& offset);

    /**
     * @brief 获取速度
     *
     * @return sf::Vector2f 速度
     */
    sf::Vector2f getVelocity() const;

    /**
     * @brief 设置速度
     *
     * @param velocity 新速度
     */
    void setVelocity(const sf::Vector2f& velocity);

    /**
     * @brief 检查是否在地面上
     *
     * @return true 在地面上
     * @return false 在空中
     */
    bool isOnGround() const;

protected:
    sf::Vector2f m_position;  ///< 位置
    sf::Vector2f m_velocity;  ///< 速度
    bool m_onGround;          ///< 是否在地面上
};

} // namespace SamuraiFight