#include "FrameTimer.h"

namespace SamuraiFight {

FrameTimer::FrameTimer()
    : m_accumulator(0.0f) {
}

void FrameTimer::update() {
    // 获取流逝时间并转换为秒
    float elapsed = m_clock.restart().asSeconds();

    // 累积时间
    m_accumulator += elapsed;
}

bool FrameTimer::shouldUpdate() const {
    return m_accumulator >= FRAME_TIME;
}

void FrameTimer::decreaseAccumulator() {
    m_accumulator -= FRAME_TIME;

    // 防止累积时间过小导致数值误差
    if (m_accumulator < 0.0f) {
        m_accumulator = 0.0f;
    }
}

float FrameTimer::getInterpolationFactor() const {
    return m_accumulator / FRAME_TIME;
}

} // namespace SamuraiFight