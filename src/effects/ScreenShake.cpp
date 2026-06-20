#include "ScreenShake.h"
#include <cstdlib>
#include <ctime>

namespace SamuraiFight {

ScreenShake::ScreenShake()
    : m_intensity(0.0f), m_duration(0.0f), m_maxDuration(0.0f), m_offset(0.0f, 0.0f) {
    // 初始化随机数种子
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
}

void ScreenShake::trigger(float intensity, float duration) {
    m_intensity = intensity;
    m_duration = duration;
    m_maxDuration = duration;
}

void ScreenShake::update(float deltaTime) {
    if (m_duration <= 0.0f) {
        m_offset = sf::Vector2f(0.0f, 0.0f);
        return;
    }

    // 更新剩余时间
    m_duration -= deltaTime;
    if (m_duration < 0.0f) {
        m_duration = 0.0f;
    }

    // 计算衰减后的强度
    float progress = 1.0f - (m_duration / m_maxDuration);
    float currentIntensity = m_intensity * (1.0f - progress);

    // 生成随机偏移
    float halfIntensity = currentIntensity * 0.5f;
    float randX = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    float randY = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    m_offset.x = (randX * 2.0f - 1.0f) * currentIntensity;
    m_offset.y = (randY * 2.0f - 1.0f) * halfIntensity; // Y方向震动幅度较小
}

sf::Vector2f ScreenShake::getOffset() const {
    return m_offset;
}

bool ScreenShake::isActive() const {
    return m_duration > 0.0f;
}

} // namespace SamuraiFight
