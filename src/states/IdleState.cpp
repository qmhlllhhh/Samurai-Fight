#include "IdleState.h"
#include "../entities/Character.h"

namespace SamuraiFight {

IdleState::IdleState(Character *owner)
    : CharacterState(owner, CharacterStateType::Idle) {
}

IdleState::~IdleState() {
}

void IdleState::onEnter() {
    CharacterState::onEnter();
    m_owner->playAnimation("idle");

    // 如果在地面上，水平速度为0;空中则保持
    sf::Vector2f vel = m_owner->getVelocity();
    if (m_owner->isOnGround()) {
        m_owner->setVelocity(sf::Vector2f(0.0f, vel.y));
    }
}

void IdleState::onExit() {
    CharacterState::onExit();
}

void IdleState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 站立时恢复体力（阶段3实现）
    // m_owner->recoverStamina(deltaTime);
}

} // namespace SamuraiFight