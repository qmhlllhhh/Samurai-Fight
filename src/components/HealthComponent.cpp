#include "HealthComponent.h"
#include <algorithm>

namespace SamuraiFight {

HealthComponent::HealthComponent(float maxHp)
    : m_currentHp(maxHp)
    , m_maxHp(maxHp) {
}

HealthComponent::~HealthComponent() {
}

float HealthComponent::takeDamage(float damage) {
    if (damage <= 0.0f) {
        return 0.0f;
    }

    float actualDamage = std::min(damage, m_currentHp);
    m_currentHp -= actualDamage;

    // 确保生命值不低于0
    m_currentHp = std::max(0.0f, m_currentHp);

    return actualDamage;
}

float HealthComponent::heal(float amount) {
    if (amount <= 0.0f) {
        return 0.0f;
    }

    float actualHeal = std::min(amount, m_maxHp - m_currentHp);
    m_currentHp += actualHeal;

    return actualHeal;
}

float HealthComponent::getCurrentHp() const {
    return m_currentHp;
}

float HealthComponent::getMaxHp() const {
    return m_maxHp;
}

float HealthComponent::getHpPercent() const {
    if (m_maxHp <= 0.0f) {
        return 0.0f;
    }
    return m_currentHp / m_maxHp;
}

bool HealthComponent::isDead() const {
    return m_currentHp <= 0.0f;
}

void HealthComponent::reset() {
    m_currentHp = m_maxHp;
}

void HealthComponent::setMaxHp(float maxHp) {
    m_maxHp = maxHp;
    // 如果当前生命值超过新的最大值，调整当前生命值
    m_currentHp = std::min(m_currentHp, m_maxHp);
}

} // namespace SamuraiFight
