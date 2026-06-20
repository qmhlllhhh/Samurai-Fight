#include "MatchManager.h"
#include "RoundManager.h"
#include <iostream>

namespace SamuraiFight {

MatchManager::MatchManager(Character* p1, Character* p2, int roundTime, int winsNeeded)
    : m_players{p1, p2}
    , m_roundTime(roundTime)
    , m_winsNeeded(winsNeeded)
    , m_currentRound(nullptr)
    , m_roundsWon{0, 0}
    , m_roundNumber(0)
    , m_matchState(MatchState::InProgress)
    , m_matchWinner(-1) {
    // 构造后自动开始第一回合
    startNextRound();
}

MatchManager::~MatchManager() {
}

void MatchManager::startNextRound() {
    if (m_matchState == MatchState::MatchOver) {
        return;
    }

    m_roundNumber++;
    m_currentRound = std::make_unique<RoundManager>(
        m_players[0], m_players[1], m_roundTime, m_roundNumber);
    m_currentRound->start();

    std::cout << "MatchManager: Starting round " << m_roundNumber
              << " (score " << m_roundsWon[0] << "-" << m_roundsWon[1] << ")" << std::endl;
}

void MatchManager::update(float deltaTime) {
    if (!m_currentRound || m_matchState == MatchState::MatchOver) {
        return;
    }

    m_currentRound->update(deltaTime);

    // 当前回合结束 → 计分 → 判断比赛是否结束 → 否则开下一回合
    if (m_currentRound->isFinished()) {
        RoundResult result = m_currentRound->getResult();

        if (result == RoundResult::Player1Win) {
            m_roundsWon[0]++;
        } else if (result == RoundResult::Player2Win) {
            m_roundsWon[1]++;
        }
        // Draw（平局）：双方都不加分

        std::cout << "MatchManager: Round " << m_roundNumber << " settled. Score "
                  << m_roundsWon[0] << "-" << m_roundsWon[1] << std::endl;

        if (m_roundsWon[0] >= m_winsNeeded) {
            m_matchWinner = 0;
            m_matchState = MatchState::MatchOver;
            std::cout << "MatchManager: Player 1 wins the match!" << std::endl;
        } else if (m_roundsWon[1] >= m_winsNeeded) {
            m_matchWinner = 1;
            m_matchState = MatchState::MatchOver;
            std::cout << "MatchManager: Player 2 wins the match!" << std::endl;
        } else {
            // 比赛未结束，开始下一回合
            startNextRound();
        }
    }
}

RoundManager* MatchManager::getCurrentRound() const {
    return m_currentRound.get();
}

int MatchManager::getRoundsWon(int playerIndex) const {
    if (playerIndex < 0 || playerIndex > 1) {
        return 0;
    }
    return m_roundsWon[playerIndex];
}

int MatchManager::getRoundNumber() const {
    return m_roundNumber;
}

MatchState MatchManager::getMatchState() const {
    return m_matchState;
}

int MatchManager::getMatchWinner() const {
    return m_matchWinner;
}

} // namespace SamuraiFight
