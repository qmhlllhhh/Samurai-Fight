#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include "FrameTimer.h"

namespace SamuraiFight {

/**
 * @file GameLoop.h
 * @brief 游戏循环控制类
 */

/**
 * @class GameLoop
 * @brief 管理游戏主循环
 *
 * 实现固定时间步长的游戏循环模式：
 * - 事件处理
 * - 游戏逻辑更新（固定60 FPS）
 * - 渲染
 */
class GameLoop {
public:
    /**
     * @brief 构造函数
     */
    GameLoop();

    /**
     * @brief 设置游戏循环的回调函数
     *
     * @param handleEventsFn 事件处理回调
     * @param updateFn 逻辑更新回调
     * @param renderFn 渲染回调
     */
    void setCallbacks(
        std::function<void()> handleEventsFn,
        std::function<void(float)> updateFn,
        std::function<void()> renderFn
    );

    /**
     * @brief 运行游戏循环
     *
     * @param window 游戏窗口
     */
    void run(sf::RenderWindow& window);

private:
    /**
     * @brief 处理窗口事件
     *
     * @param window 游戏窗口
     */
    void handleEvents(sf::RenderWindow& window);

    /**
     * @brief 更新游戏逻辑
     *
     * @param deltaTime 固定时间步长
     */
    void update(float deltaTime);

    /**
     * @brief 渲染画面
     */
    void render();

    FrameTimer m_frameTimer;                    ///< 帧计时器
    std::function<void()> m_handleEventsFn;     ///< 事件处理回调
    std::function<void(float)> m_updateFn;      ///< 逻辑更新回调
    std::function<void()> m_renderFn;           ///< 渲染回调
    bool m_running;                             ///< 运行状态标志
};

} // namespace SamuraiFight