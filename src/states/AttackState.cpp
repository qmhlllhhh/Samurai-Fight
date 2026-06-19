#include "AttackState.h"
#include "../entities/Character.h"
#include "../entities/CharacterData.h"
#include <iostream>

namespace SamuraiFight {

AttackState::AttackState(Character* owner, const std::string& attackType)
    : CharacterState(owner, CharacterStateType::AttackLight)  // 默认设置为轻攻击，后面会根据类型更新
    , m_attackType(attackType)
    , m_totalFrames(10)
    , m_startupEnd(3)
    , m_activeEnd(5)
    , m_hasHit(false)
    , m_canCancel(true)
    , m_cancelWindowStart(2)
    , m_cancelWindowEnd(5)
    , m_maxHitCount(1)
    , m_currentHitCount(0) {
    // 根据攻击类型设置状态类型
    if (attackType == "light") {
        m_type = CharacterStateType::AttackLight;
    } else if (attackType == "medium") {
        m_type = CharacterStateType::AttackMedium;
    } else if (attackType == "heavy") {
        m_type = CharacterStateType::AttackHeavy;
    }

    // 从角色数据加载攻击帧数据
    const CharacterData& data = owner->getData();
    auto it = data.attacks.find(attackType);
    if (it != data.attacks.end()) {
        const AttackData& attackData = it->second;
        m_totalFrames = attackData.getTotalFrames();
        m_startupEnd = attackData.startupFrames;
        m_activeEnd = attackData.startupFrames + attackData.activeFrames;
        m_canCancel = attackData.canCancel;
        m_cancelWindowStart = attackData.cancelWindowStart;
        m_cancelWindowEnd = attackData.cancelWindowEnd;
        m_maxHitCount = attackData.hitCount;
    }
}

AttackState::~AttackState() {
}

void AttackState::onEnter() {
    CharacterState::onEnter();

    // 播放对应动画
    std::string animName = "attack_" + m_attackType;
    m_owner->playAnimation(animName);

    // 停止水平移动
    sf::Vector2f vel = m_owner->getVelocity();
    m_owner->setVelocity(sf::Vector2f(0.0f, vel.y));

    // 重置状态
    m_hasHit = false;
    m_frameCount = 0;

    std::cout << "AttackState: Entered " << m_attackType << " attack"
              << " (startup=" << m_startupEnd
              << ", active=" << (m_activeEnd - m_startupEnd)
              << ", recovery=" << (m_totalFrames - m_activeEnd)
              << ", total=" << m_totalFrames << ")" << std::endl;
}

void AttackState::onExit() {
    CharacterState::onExit();
}

void AttackState::update(float deltaTime) {
    CharacterState::update(deltaTime);

    // 检查攻击是否完成
    if (m_frameCount >= m_totalFrames) {
        // 攻击结束，回到站立状态
        m_owner->changeState(CharacterStateType::Idle);
        return;
    }
}

const std::string& AttackState::getAttackType() const {
    return m_attackType;
}

bool AttackState::isInActiveFrames() const {
    // 判定帧区间: [m_startupEnd, m_activeEnd)
    return m_frameCount >= m_startupEnd && m_frameCount < m_activeEnd;
}

std::pair<int, int> AttackState::getActiveFrameRange() const {
    return {m_startupEnd, m_activeEnd - 1};  // 返回闭区间
}

bool AttackState::isFrameInActiveFrames(int frame) const {
    return frame >= m_startupEnd && frame < m_activeEnd;
}

bool AttackState::canCancel() const {
    if (!m_canCancel) {
        return false;
    }
    // 取消窗口: [m_cancelWindowStart, m_cancelWindowEnd]
    return m_frameCount >= m_cancelWindowStart && m_frameCount <= m_cancelWindowEnd;
}

bool AttackState::isFinished() const {
    return m_frameCount >= m_totalFrames;
}

int AttackState::getPhase() const {
    if (m_frameCount < m_startupEnd) {
        return 0;  // 前摇
    } else if (m_frameCount < m_activeEnd) {
        return 1;  // 判定
    } else {
        return 2;  // 后摇
    }
}

int AttackState::getCurrentAttackFrame() const {
    return m_frameCount;
}

int AttackState::getMaxHitCount() const {
    return m_maxHitCount;
}

int AttackState::getCurrentHitCount() const {
    return m_currentHitCount;
}

void AttackState::incrementHitCount() {
    m_currentHitCount++;
}

bool AttackState::canHit() const {
    return m_currentHitCount < m_maxHitCount;
}

} // namespace SamuraiFight
