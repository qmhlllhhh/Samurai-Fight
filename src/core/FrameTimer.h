#pragma once

#include <SFML/System.hpp>
#include "Constants.h"

namespace SamuraiFight {

/**
 * @file FrameTimer.h
 * @brief 帧计时器，实现固定时间步长游戏循环
 */

/**
 * @class FrameTimer
 * @brief 60 FPS固定时间步长计时器
 *
 * 使用累积时间方法确保游戏逻辑更新频率恒定，
 * 无论实际帧率如何波动。
 */
class FrameTimer {
public:
    /**
     * @brief 构造函数
     */
    FrameTimer();

    /**
     * @brief 更新累积时间
     *
     * 从时钟获取流逝时间并累积
     */
    void update();

    /**
     * @brief 判断是否应该执行逻辑更新
     *
     * @return true 如果累积时间超过一帧时间
     * @return false 否则
     */
    bool shouldUpdate() const;

    /**
     * @brief 消耗一帧时间
     *
     * 从累积时间中减去一帧时间
     */
    void decreaseAccumulator();

    /**
     * @brief 获取当前的插值因子
     *
     * 用于平滑渲染，表示当前帧进度
     *
     * @return float 插值因子 (0.0 - 1.0)
     */
    float getInterpolationFactor() const;

private:
    float m_accumulator;      ///< 累积时间
    sf::Clock m_clock;        ///< SFML时钟
};

} // namespace SamuraiFight