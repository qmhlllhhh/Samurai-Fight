#include "RunState.h"
#include "../entities/Character.h"
#include "../core/Constants.h"

namespace SamuraiFight {

RunState::RunState(Character* owner)
    : CharacterState(owner, CharacterStateType::Run)
    , m_direction(1)
    , m_runSpeed(0.0f)
    , m_staminaCostPerFrame(0.3f) {
}

RunState::~RunState() {
}

void RunState::onEnter() {
    CharacterState::onEnter();

    // 播放跑步动画
    m_owner->playAnimation("run");

    // 跑步速度为走路速度的1.8倍
    m_runSpeed = m_owner->getMoveSpeed() * 1.8f;
}

void RunState::onExit() {
    CharacterState::onExit();
}

void RunState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 根据朝向确定跑步方向
    m_direction = m_owner->isFacingRight() ? 1 : -1;

    // 设置水平速度
    sf::Vector2f velocity = m_owner->getVelocity();
    velocity.x = m_direction * m_runSpeed;
    m_owner->setVelocity(velocity);

    // 消耗体力
    if (m_owner->getStaminaComponent()) {
        m_owner->getStaminaComponent()->consume(m_staminaCostPerFrame);
    }
}

} // namespace SamuraiFight
