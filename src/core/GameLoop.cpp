#include "GameLoop.h"

#include <iostream>

namespace SamuraiFight {

GameLoop::GameLoop()
    : m_running(false)
    , m_handleEventsFn(nullptr)
    , m_updateFn(nullptr)
    , m_renderFn(nullptr) {
}

void GameLoop::setCallbacks(
    std::function<void()> handleEventsFn,
    std::function<void(float)> updateFn,
    std::function<void()> renderFn
) {
    m_handleEventsFn = handleEventsFn;
    m_updateFn = updateFn;
    m_renderFn = renderFn;
}

void GameLoop::run(sf::RenderWindow& window) {
    m_running = true;

    while (m_running && window.isOpen()) {
        // 更新帧计时器
        m_frameTimer.update();

        // 固定时间步长逻辑更新
        while (m_frameTimer.shouldUpdate()) {
            // 处理事件
            handleEvents(window);

            // 更新游戏逻辑
            update(FRAME_TIME);

            // 消耗累积时间
            m_frameTimer.decreaseAccumulator();
        }

        // 渲染
        render();
    }
}

void GameLoop::handleEvents(sf::RenderWindow& window) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        // 处理窗口关闭事件
        if (event->is<sf::Event::Closed>()) {
            window.close();
            m_running = false;
            return;
        }

        // 调用外部事件处理回调
        if (m_handleEventsFn) {
            // 由于SFML 3.0的事件系统改变，我们需要传递事件对象
            // 这里简化处理，让外部回调处理具体逻辑
        }
    }

    // 如果设置了事件处理回调，调用它
    if (m_handleEventsFn) {
        m_handleEventsFn();
    }
}

void GameLoop::update(float deltaTime) {
    if (m_updateFn) {
        m_updateFn(deltaTime);
    }
}

void GameLoop::render() {
    if (m_renderFn) {
        m_renderFn();
    }
}

} // namespace SamuraiFight