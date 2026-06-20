#include "BlockState.h"
#include "../entities/Character.h"
#include "../managers/AudioManager.h"
#include <iostream>

namespace SamuraiFight {

BlockState::BlockState(Character *owner)
    : CharacterState(owner, CharacterStateType::Block), m_blockSuccess(false) {
}

BlockState::~BlockState() {
}

void BlockState::onEnter() {
    CharacterState::onEnter();

    // 播放防御动画
    m_owner->playAnimation("block");

    // 停止水平移动
    sf::Vector2f vel = m_owner->getVelocity();
    m_owner->setVelocity(sf::Vector2f(0.0f, vel.y));

    // 重置状态
    m_blockSuccess = false;

    std::cout << "BlockState: Entered block state" << std::endl;
}

void BlockState::onExit() {
    CharacterState::onExit();

    // 如果是被打破防御，不设置冷却
    if (!m_blockSuccess) {
        // 正常退出防御，设置冷却时间
        m_owner->setBlockCooldown(true);
    }

    std::cout << "BlockState: Exited block state (success: " << (m_blockSuccess ? "yes" : "no") << ")" << std::endl;
}

void BlockState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 检查体力是否完全耗尽（只有清空时才强制退出）
    if (m_owner->getStaminaComponent() && m_owner->getStaminaComponent()->isExhausted()) {
        std::cout << "BlockState: Stamina exhausted, exiting block" << std::endl;
        m_owner->changeState(CharacterStateType::Idle);
        return;
    }

    // 持续消耗体力
    if (m_owner->getStaminaComponent()) {
        m_owner->getStaminaComponent()->consume(0.5f); // 每帧消耗0.5体力
    }

    // // 如果被打破防御，退出状态
    // if (m_blockSuccess) {
    //     m_owner->changeState(CharacterStateType::Idle);
    //     return;
    // }
}

bool BlockState::hasEnoughStamina() const {
    if (m_owner->getStaminaComponent()) {
        return m_owner->getStaminaComponent()->getCurrentStamina() > 0.0f;
    }
    return false;
}

void BlockState::onBlockHit() {
    m_blockSuccess = true;

    // 播放格挡成功音效
    AudioManager::getInstance().playSound("block_success");

    std::cout << "BlockState: Block success!" << std::endl;
}

bool BlockState::isSuccess() const {
    return m_blockSuccess;
}

} // namespace SamuraiFight
