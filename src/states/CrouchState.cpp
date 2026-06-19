#include "CrouchState.h"
#include "../entities/Character.h"

namespace SamuraiFight {

CrouchState::CrouchState(Character* owner)
    : CharacterState(owner, CharacterStateType::Crouch) {
}

CrouchState::~CrouchState() {
}

void CrouchState::onEnter() {
    CharacterState::onEnter();
    m_owner->playAnimation("crouch");

    // 停止移动
    m_owner->setVelocity(sf::Vector2f(0.0f, m_owner->getVelocity().y));
}

void CrouchState::onExit() {
    CharacterState::onExit();
}

void CrouchState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 下蹲时不做特殊处理
    // 可以在这里添加下蹲相关的逻辑
}

} // namespace SamuraiFight