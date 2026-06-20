#include "WalkState.h"
#include "../entities/Character.h"
#include "../core/Constants.h"

namespace SamuraiFight {

WalkState::WalkState(Character* owner)
    : CharacterState(owner, CharacterStateType::Walk)
    , m_direction(1) {
}

WalkState::~WalkState() {
}

void WalkState::onEnter() {
    CharacterState::onEnter();
    m_owner->playAnimation("walk");
}

void WalkState::onExit() {
    CharacterState::onExit();
}

void WalkState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 根据朝向确定移动方向
    m_direction = m_owner->isFacingRight() ? 1 : -1;

    // 设置水平速度
    float moveSpeed = m_owner->getMoveSpeed();
    sf::Vector2f velocity = m_owner->getVelocity();
    velocity.x = m_direction * moveSpeed;
    m_owner->setVelocity(velocity);
}

} // namespace SamuraiFight