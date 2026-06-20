#include "HurtState.h"
#include "../entities/Character.h"
#include <iostream>

namespace SamuraiFight {

HurtState::HurtState(Character *owner, int stunFrames)
    : CharacterState(owner, CharacterStateType::Hurt), m_stunFrames(stunFrames), m_stunFinished(false) {
}

HurtState::~HurtState() {
}

void HurtState::onEnter() {
    CharacterState::onEnter();

    // 播放受击动画
    m_owner->playAnimation("hurt");

    // 如果在地面上，水平速度为0;空中则保持
    sf::Vector2f vel = m_owner->getVelocity();
    if (m_owner->isOnGround()) {
        m_owner->setVelocity(sf::Vector2f(0.0f, vel.y));
    }

    // 重置状态
    m_stunFinished = false;
    m_frameCount = 0;

    std::cout << "HurtState: Entered with " << m_stunFrames << " frames stun" << std::endl;
}

void HurtState::onExit() {
    CharacterState::onExit();
    std::cout << "HurtState: Exited" << std::endl;
}

void HurtState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 检查硬直是否结束
    if (m_frameCount >= m_stunFrames) {
        m_stunFinished = true;
        // 硬直结束，回到站立状态
        m_owner->changeState(CharacterStateType::Idle);
    }
}

int HurtState::getStunFrames() const {
    return m_stunFrames;
}

bool HurtState::isStunFinished() const {
    return m_stunFinished;
}

} // namespace SamuraiFight
