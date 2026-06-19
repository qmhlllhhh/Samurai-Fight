#include "StaminaComponent.h"
#include <algorithm>

namespace SamuraiFight {

StaminaComponent::StaminaComponent(float maxStamina)
    : m_currentStamina(maxStamina)
    , m_maxStamina(maxStamina)
    , m_wasJumping(false) {
}

StaminaComponent::~StaminaComponent() {
}

float StaminaComponent::consume(float amount) {
    if (amount <= 0.0f) {
        return 0.0f;
    }

    float actualConsume = std::min(amount, m_currentStamina);
    m_currentStamina -= actualConsume;

    // 确保体力值不低于0
    m_currentStamina = std::max(0.0f, m_currentStamina);

    return actualConsume;
}

float StaminaComponent::recover(float amount) {
    if (amount <= 0.0f) {
        return 0.0f;
    }

    float actualRecover = std::min(amount, m_maxStamina - m_currentStamina);
    m_currentStamina += actualRecover;

    return actualRecover;
}

void StaminaComponent::update(bool isMoving, bool isJumping, bool isStanding,
                               float moveCostPerFrame, float recoveryPerFrame, float jumpCost) {
    // 处理跳跃消耗（只在跳跃开始时消耗）
    if (isJumping && !m_wasJumping) {
        consume(jumpCost);
    }
    m_wasJumping = isJumping;

    // 处理移动消耗
    if (isMoving) {
        consume(moveCostPerFrame);
    }

    // 站立时恢复体力
    if (isStanding && !isMoving) {
        recover(recoveryPerFrame);
    }
}

float StaminaComponent::getCurrentStamina() const {
    return m_currentStamina;
}

float StaminaComponent::getMaxStamina() const {
    return m_maxStamina;
}

float StaminaComponent::getStaminaPercent() const {
    if (m_maxStamina <= 0.0f) {
        return 0.0f;
    }
    return m_currentStamina / m_maxStamina;
}

bool StaminaComponent::isExhausted() const {
    return m_currentStamina <= 0.0f;
}

bool StaminaComponent::hasEnoughStamina(float amount) const {
    return m_currentStamina >= amount;
}

void StaminaComponent::reset() {
    m_currentStamina = m_maxStamina;
}

void StaminaComponent::setMaxStamina(float maxStamina) {
    m_maxStamina = maxStamina;
    // 如果当前体力值超过新的最大值，调整当前体力值
    m_currentStamina = std::min(m_currentStamina, m_maxStamina);
}

} // namespace SamuraiFight
