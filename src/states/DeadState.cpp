#include "DeadState.h"
#include "../entities/Character.h"
#include <iostream>

namespace SamuraiFight {

DeadState::DeadState(Character* owner)
    : CharacterState(owner, CharacterStateType::Dead)
    , m_animationFinished(false) {
}

DeadState::~DeadState() {
}

void DeadState::onEnter() {
    CharacterState::onEnter();

    // 播放死亡动画
    m_owner->playAnimation("death");

    // 停止所有移动
    m_owner->setVelocity(sf::Vector2f(0.0f, 0.0f));

    // 重置状态
    m_animationFinished = false;
    m_frameCount = 0;

    std::cout << "DeadState: Character " << m_owner->getPlayerIndex() << " has died" << std::endl;
}

void DeadState::onExit() {
    CharacterState::onExit();
    std::cout << "DeadState: Exited" << std::endl;
}

void DeadState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 检查动画是否播放完成
    // 死亡动画不循环，播放完成后停留在最后一帧
    if (!m_animationFinished) {
        // 获取动画组件检查是否播放完成
        // 由于死亡动画设置为不循环，动画会自动停留在最后一帧
        // 我们只需要标记动画已完成
        m_animationFinished = true;
    }

    // 死亡状态不自动退出，角色保持死亡状态
    // 不调用 changeState，保持当前状态
}

bool DeadState::isAnimationFinished() const {
    return m_animationFinished;
}

} // namespace SamuraiFight
