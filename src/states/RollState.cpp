#include "RollState.h"
#include "../entities/Character.h"
#include "../core/Constants.h"
#include <iostream>

namespace SamuraiFight {

RollState::RollState(Character* owner)
    : CharacterState(owner, CharacterStateType::Roll)
    , m_totalFrames(20)      // 翻滚持续20帧
    , m_rollSpeed(600.0f)    // 翻滚速度（比走路快）
    , m_facingRight(true) {
}

RollState::~RollState() {
}

void RollState::onEnter() {
    CharacterState::onEnter();

    // 播放翻滚动画
    m_owner->playAnimation("roll");

    // 记录进入时的朝向
    m_facingRight = m_owner->isFacingRight();

    // 设置翻滚速度（水平方向）
    sf::Vector2f velocity = m_owner->getVelocity();
    if (m_facingRight) {
        velocity.x = m_rollSpeed;
    } else {
        velocity.x = -m_rollSpeed;
    }
    m_owner->setVelocity(velocity);

    // 消耗体力
    if (m_owner->getStaminaComponent()) {
        m_owner->getStaminaComponent()->consume(15.0f);  // 翻滚消耗15体力
    }

    // 重置帧计数
    m_frameCount = 0;

    std::cout << "RollState: Entered roll state, direction: " << (m_facingRight ? "right" : "left") << std::endl;
}

void RollState::onExit() {
    CharacterState::onExit();
    std::cout << "RollState: Exited roll state" << std::endl;
}

void RollState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 保持翻滚速度（不受其他输入影响）
    sf::Vector2f velocity = m_owner->getVelocity();
    if (m_facingRight) {
        velocity.x = m_rollSpeed;
    } else {
        velocity.x = -m_rollSpeed;
    }
    m_owner->setVelocity(velocity);

    // 检查翻滚是否结束
    if (m_frameCount >= m_totalFrames) {
        // 翻滚结束，回到站立状态
        m_owner->changeState(CharacterStateType::Idle);
    }
}

int RollState::getTotalFrames() const {
    return m_totalFrames;
}

} // namespace SamuraiFight
