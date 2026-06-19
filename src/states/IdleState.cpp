#include "IdleState.h"
#include "../entities/Character.h"

namespace SamuraiFight {

IdleState::IdleState(Character* owner)
    : CharacterState(owner, CharacterStateType::Idle) {
}

IdleState::~IdleState() {
}

void IdleState::onEnter() {
    CharacterState::onEnter();
    m_owner->playAnimation("idle");

    // 停止移动
    m_owner->setVelocity(sf::Vector2f(0.0f, m_owner->getVelocity().y));
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