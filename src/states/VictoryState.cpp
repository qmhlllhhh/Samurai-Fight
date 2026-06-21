#include "VictoryState.h"
#include "../entities/Character.h"
#include <iostream>

namespace SamuraiFight {

VictoryState::VictoryState(Character* owner)
    : CharacterState(owner, CharacterStateType::Victory) {
}

VictoryState::~VictoryState() {
}

void VictoryState::onEnter() {
    CharacterState::onEnter();

    // 播放胜利动画（若无专用素材则保持当前动画）
    m_owner->playAnimation("victory");

    // 停止所有移动
    m_owner->setVelocity(sf::Vector2f(0.0f, 0.0f));

    m_frameCount = 0;

    std::cout << "VictoryState: Player " << m_owner->getPlayerIndex() + 1 << " wins!" << std::endl;
}

void VictoryState::onExit() {
    CharacterState::onExit();
    std::cout << "VictoryState: Exited" << std::endl;
}

void VictoryState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 胜利状态不自动退出，保持胜利姿态直到比赛结束切场景
}

} // namespace SamuraiFight
