#include "CharacterState.h"

namespace SamuraiFight {

CharacterState::CharacterState(Character* owner, CharacterStateType type)
    : m_owner(owner)
    , m_type(type)
    , m_frameCount(0) {
}

CharacterState::~CharacterState() {
}

void CharacterState::onEnter() {
    m_frameCount = 0;
}

void CharacterState::onExit() {
    // 默认实现：空操作
}

void CharacterState::update(float /*deltaTime*/) {
    m_frameCount++;
}

CharacterStateType CharacterState::getType() const {
    return m_type;
}

Character* CharacterState::getOwner() const {
    return m_owner;
}

} // namespace SamuraiFight