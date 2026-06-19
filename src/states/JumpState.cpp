#include "JumpState.h"
#include "../entities/Character.h"
#include "../core/Constants.h"
#include <iostream>

namespace SamuraiFight {

JumpState::JumpState(Character* owner)
    : CharacterState(owner, CharacterStateType::Jump)
    , m_isFalling(false) {
}

JumpState::~JumpState() {
}

void JumpState::onEnter() {
    CharacterState::onEnter();
    m_owner->playAnimation("jump");
    m_isFalling = false;

    // 设置跳跃初速度
    float jumpHeight = m_owner->getJumpHeight();
    sf::Vector2f velocity = m_owner->getVelocity();
    velocity.y = -jumpHeight; // 向上跳跃
    m_owner->setVelocity(velocity);

    std::cout << "JumpState: onEnter - velocity.y = " << velocity.y << std::endl;

    // 消耗体力（阶段3实现）
    // m_owner->consumeStamina(JUMP_STAMINA_COST);
}

void JumpState::onExit() {
    CharacterState::onExit();
}

void JumpState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 空中移动控制
    // 允许在空中使用左右键调整水平速度
    // 这里需要从Character获取输入状态，通过公开的方法或者直接访问
    // 由于Character没有提供获取输入状态的公开方法，我们通过速度来处理

    // 获取移动速度（空中移动速度可以稍微降低）
    float moveSpeed = m_owner->getMoveSpeed() * 0.8f; // 空中移动速度降低20%

    // 获取当前速度
    sf::Vector2f velocity = m_owner->getVelocity();

    // 保持朝向，水平速度由 Character::handleInput 处理
    // 在跳跃状态时，handleInput 会更新朝向并允许水平移动

    // 检查是否开始下落
    if (velocity.y > 0) {
        m_isFalling = true;
    }

    // 检测落地
    if (m_owner->isOnGround()) {
        std::cout << "JumpState: Landed - switching to Idle" << std::endl;
        // 落地后切换回站立状态
        m_owner->changeState(CharacterStateType::Idle);
    }
}

} // namespace SamuraiFight