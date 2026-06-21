#include "RoundManager.h"
#include "../components/HealthComponent.h"
#include "../entities/Character.h"
#include <cmath>
#include <iostream>

namespace SamuraiFight {

namespace {
// 各阶段持续时间（秒），60 帧 = 1 秒
constexpr float INTRO_DURATION = 1.0f;      ///< Intro："ROUND n"
constexpr float READY_DURATION = 0.5f;      ///< Ready："FIGHT!"
constexpr float ROUND_OVER_DURATION = 3.0f; ///< RoundOver："K.O."/"TIME UP" 停留
} // namespace

RoundManager::RoundManager(Character *p1, Character *p2, int roundTime, int roundNumber)
    : m_players{p1, p2}, m_roundTime(roundTime), m_roundNumber(roundNumber), m_phase(RoundPhase::Intro), m_phaseTimer(0.0f), m_timeRemaining(static_cast<float>(roundTime)), m_result(RoundResult::None), m_timeUp(false) {
}

RoundManager::~RoundManager() {
}

void RoundManager::start() {
    resetCharacters();
    m_timeRemaining = static_cast<float>(m_roundTime);
    m_result = RoundResult::None;
    m_timeUp = false;
    setPhase(RoundPhase::Intro);

    std::cout << "RoundManager: Round " << m_roundNumber << " started" << std::endl;
}

void RoundManager::update(float deltaTime) {
    m_phaseTimer += deltaTime;

    switch (m_phase) {
    case RoundPhase::Intro:
        if (m_phaseTimer >= INTRO_DURATION) {
            setPhase(RoundPhase::Ready);
        }
        break;
    case RoundPhase::Ready:
        if (m_phaseTimer >= READY_DURATION) {
            setPhase(RoundPhase::Fight);
        }
        break;
    case RoundPhase::Fight:
        m_timeRemaining -= deltaTime;
        if (m_timeRemaining < 0.0f) {
            m_timeRemaining = 0.0f;
        }
        checkRoundEnd();
        break;
    case RoundPhase::RoundOver:
        // 等待停留结束，由 isFinished() 判断
        break;
    }
}

bool RoundManager::isInputLocked() const {
    // 仅 Fight 阶段允许玩家输入
    return m_phase != RoundPhase::Fight;
}

bool RoundManager::isFinished() const {
    return m_phase == RoundPhase::RoundOver && m_phaseTimer >= ROUND_OVER_DURATION;
}

RoundResult RoundManager::getResult() const {
    return m_result;
}

bool RoundManager::isTimeUp() const {
    return m_timeUp;
}

int RoundManager::getTimeRemaining() const {
    return static_cast<int>(std::ceil(m_timeRemaining));
}

RoundPhase RoundManager::getPhase() const {
    return m_phase;
}

int RoundManager::getRoundNumber() const {
    return m_roundNumber;
}

void RoundManager::setPhase(RoundPhase phase) {
    m_phase = phase;
    m_phaseTimer = 0.0f;
}

void RoundManager::checkRoundEnd() {
    if (m_result != RoundResult::None) {
        return; // 本回合已判定
    }

    bool p1Dead = m_players[0] && m_players[0]->isDead();
    bool p2Dead = m_players[1] && m_players[1]->isDead();

    if (p1Dead && p2Dead) {
        // 同帧双杀，判平局
        m_result = RoundResult::Draw;
    } else if (p1Dead) {
        m_result = RoundResult::Player2Win;
    } else if (p2Dead) {
        m_result = RoundResult::Player1Win;
    } else if (m_timeRemaining <= 0.0f) {
        // 时间到，比较生命值
        m_timeUp = true;
        float hp1 = m_players[0] ? m_players[0]->getHealthComponent()->getCurrentHp() : 0.0f;
        float hp2 = m_players[1] ? m_players[1]->getHealthComponent()->getCurrentHp() : 0.0f;
        if (hp1 > hp2) {
            m_result = RoundResult::Player1Win;
        } else if (hp2 > hp1) {
            m_result = RoundResult::Player2Win;
        } else {
            m_result = RoundResult::Draw;
        }
    }

    if (m_result != RoundResult::None) {
        setPhase(RoundPhase::RoundOver);

        const char *resultStr = "Unknown";
        switch (m_result) {
        case RoundResult::Player1Win:
            resultStr = "Player1Win";
            break;
        case RoundResult::Player2Win:
            resultStr = "Player2Win";
            break;
        case RoundResult::Draw:
            resultStr = "Draw";
            break;
        default:
            break;
        }
        std::cout << "RoundManager: Round " << m_roundNumber << " ended (" << resultStr << ")" << std::endl;
    }
}

void RoundManager::resetCharacters() {
    if (m_players[0]) {
        m_players[0]->resetForRound();
    }
    if (m_players[1]) {
        m_players[1]->resetForRound();
    }
}

} // namespace SamuraiFight
