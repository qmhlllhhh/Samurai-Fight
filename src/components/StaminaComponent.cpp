#include "StaminaComponent.h"
#include <algorithm>

namespace SamuraiFight {

StaminaComponent::StaminaComponent(float maxStamina)
    : m_currentStamina(maxStamina), m_maxStamina(maxStamina), m_threshold(maxStamina * 0.2f) // 阈值为最大体力的20%
      ,
      m_wasJumping(false) {
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

    // 如果体力值低于阈值（20%），恢复速度减半
    float recoveryMultiplier = isBelowThreshold() ? 0.5f : 1.0f;
    float adjustedAmount = amount * recoveryMultiplier;

    float actualRecover = std::min(adjustedAmount, m_maxStamina - m_currentStamina);
    m_currentStamina += actualRecover;

    return actualRecover;
}

void StaminaComponent::update(bool isMoving, bool isJumping, bool isStanding,
                              float moveRecoveryPerFrame, float recoveryPerFrame, float jumpCost) {
    // 处理跳跃消耗（只在跳跃开始时消耗）
    if (isJumping && !m_wasJumping) {
        consume(jumpCost);
    }
    m_wasJumping = isJumping;

    // 处理移动消耗
    if (isMoving) {
        recover(moveRecoveryPerFrame);
    }

    // 站立时恢复体力
    if (isStanding) {
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

bool StaminaComponent::isBelowThreshold() const {
    return m_currentStamina < m_threshold;
}

float StaminaComponent::getThreshold() const {
    return m_threshold;
}

bool StaminaComponent::canEnterStaminaState() const {
    // 体力值大于阈值（20%）才能进入跳跃、防御状态
    return m_currentStamina > m_threshold;
}

void StaminaComponent::reset() {
    m_currentStamina = m_maxStamina;
    m_wasJumping = false;  // 同步重置跳跃状态标志，确保 reset 彻底
}

void StaminaComponent::setMaxStamina(float maxStamina) {
    m_maxStamina = maxStamina;
    m_threshold = maxStamina * 0.2f; // 更新阈值为20%
    // 如果当前体力值超过新的最大值，调整当前体力值
    m_currentStamina = std::min(m_currentStamina, m_maxStamina);
}

} // namespace SamuraiFight
