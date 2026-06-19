#include "StateMachine.h"
#include "../entities/Character.h"
#include "IdleState.h"
#include "MoveState.h"
#include "JumpState.h"
#include "CrouchState.h"

#include <iostream>

namespace SamuraiFight {

StateMachine::StateMachine(Character* owner)
    : m_owner(owner)
    , m_currentState(nullptr)
    , m_currentStateType(CharacterStateType::Idle) {
    // 初始化为站立状态
    changeState(CharacterStateType::Idle);
}

StateMachine::~StateMachine() {
    if (m_currentState) {
        m_currentState->onExit();
    }
}

void StateMachine::changeState(CharacterStateType stateType) {
    // 如果是同一个状态，不切换
    if (m_currentState && m_currentStateType == stateType) {
        return;
    }

    // 退出当前状态
    if (m_currentState) {
        m_currentState->onExit();
    }

    // 创建新状态
    m_currentState = createState(stateType);
    m_currentStateType = stateType;

    // 进入新状态
    if (m_currentState) {
        m_currentState->onEnter();
    } else {
        std::cerr << "StateMachine: Failed to create state " << static_cast<int>(stateType) << std::endl;
    }
}

void StateMachine::update(float deltaTime) {
    if (m_currentState) {
        m_currentState->update(deltaTime);
    }
}

CharacterState* StateMachine::getCurrentState() const {
    return m_currentState.get();
}

CharacterStateType StateMachine::getCurrentStateType() const {
    return m_currentStateType;
}

std::unique_ptr<CharacterState> StateMachine::createState(CharacterStateType stateType) {
    switch (stateType) {
    case CharacterStateType::Idle:
        return std::make_unique<IdleState>(m_owner);
    case CharacterStateType::Move:
        return std::make_unique<MoveState>(m_owner);
    case CharacterStateType::Jump:
        return std::make_unique<JumpState>(m_owner);
    case CharacterStateType::Crouch:
        return std::make_unique<CrouchState>(m_owner);
    default:
        std::cerr << "StateMachine: Unknown state type " << static_cast<int>(stateType) << std::endl;
        return nullptr;
    }
}

} // namespace SamuraiFight