#include "MoveState.h"
#include "../entities/Character.h"
#include "../core/Constants.h"

namespace SamuraiFight {

MoveState::MoveState(Character* owner)
    : CharacterState(owner, CharacterStateType::Move)
    , m_direction(1) {
}

MoveState::~MoveState() {
}

void MoveState::onEnter() {
    CharacterState::onEnter();
    m_owner->playAnimation("walk");
}

void MoveState::onExit() {
    CharacterState::onExit();
}

void MoveState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 根据朝向确定移动方向
    m_direction = m_owner->isFacingRight() ? 1 : -1;

    // 设置水平速度
    float moveSpeed = m_owner->getMoveSpeed();
    sf::Vector2f velocity = m_owner->getVelocity();
    velocity.x = m_direction * moveSpeed;
    m_owner->setVelocity(velocity);

    // 移动时消耗体力（阶段3实现）
    // m_owner->consumeStamina(MOVE_STAMINA_COST);
}

} // namespace SamuraiFight