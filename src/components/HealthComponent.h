#pragma once

#include <SFML/Graphics.hpp>

namespace SamuraiFight {

/**
 * @file HealthComponent.h
 * @brief 生命值组件
 */

/**
 * @class HealthComponent
 * @brief 管理角色的生命值
 *
 * 包含当前生命值、最大生命值、伤害处理等功能
 */
class HealthComponent {
public:
    /**
     * @brief 构造函数
     *
     * @param maxHp 最大生命值
     */
    explicit HealthComponent(float maxHp = 100.0f);

    /**
     * @brief 析构函数
     */
    ~HealthComponent();

    /**
     * @brief 受到伤害
     *
     * @param damage 伤害值
     * @return float 实际造成的伤害
     */
    float takeDamage(float damage);

    /**
     * @brief 恢复生命值
     *
     * @param amount 恢复量
     * @return float 实际恢复的量
     */
    float heal(float amount);

    /**
     * @brief 获取当前生命值
     *
     * @return float 当前生命值
     */
    float getCurrentHp() const;

    /**
     * @brief 获取最大生命值
     *
     * @return float 最大生命值
     */
    float getMaxHp() const;

    /**
     * @brief 获取生命值百分比
     *
     * @return float 生命值百分比 (0.0 - 1.0)
     */
    float getHpPercent() const;

    /**
     * @brief 检查是否死亡
     *
     * @return true 死亡
     * @return false 存活
     */
    bool isDead() const;

    /**
     * @brief 重置生命值到最大值
     */
    void reset();

    /**
     * @brief 设置最大生命值
     *
     * @param maxHp 新的最大生命值
     */
    void setMaxHp(float maxHp);

private:
    float m_currentHp;  ///< 当前生命值
    float m_maxHp;      ///< 最大生命值
};

} // namespace SamuraiFight
