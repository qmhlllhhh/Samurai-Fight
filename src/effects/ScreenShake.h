#pragma once

#include <SFML/System.hpp>

namespace SamuraiFight {

/**
 * @file ScreenShake.h
 * @brief 屏幕震动效果
 */

/**
 * @class ScreenShake
 * @brief 管理屏幕震动效果
 *
 * 提供屏幕震动功能，用于受击等事件的视觉反馈
 */
class ScreenShake {
public:
    /**
     * @brief 构造函数
     */
    ScreenShake();

    /**
     * @brief 析构函数
     */
    ~ScreenShake() = default;

    /**
     * @brief 触发屏幕震动
     *
     * @param intensity 震动强度（像素）
     * @param duration 持续时间（秒）
     */
    void trigger(float intensity, float duration);

    /**
     * @brief 更新震动状态
     *
     * @param deltaTime 时间增量
     */
    void update(float deltaTime);

    /**
     * @brief 获取当前震动偏移
     *
     * @return sf::Vector2f 震动偏移
     */
    sf::Vector2f getOffset() const;

    /**
     * @brief 检查是否正在震动
     *
     * @return true 正在震动
     * @return false 未震动
     */
    bool isActive() const;

private:
    float m_intensity;     ///< 当前震动强度
    float m_duration;      ///< 剩余震动时间
    float m_maxDuration;   ///< 最大持续时间
    sf::Vector2f m_offset; ///< 当前震动偏移
};

} // namespace SamuraiFight
