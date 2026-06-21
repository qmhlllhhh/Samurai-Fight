#include "MatchHud.h"
#include "../managers/ResourceManager.h"
#include "../core/Constants.h"
#include <string>

namespace SamuraiFight {

MatchHud::MatchHud() {
    const sf::Font& font = ResourceManager::getInstance().getDefaultFont();

    // 计时器：顶部中央
    m_timer = std::make_unique<Timer>(sf::Vector2f(WINDOW_WIDTH / 2.0f, 25.0f), 48);

    // 比分：计时器下方
    m_scoreDisplay = std::make_unique<ScoreDisplay>(sf::Vector2f(WINDOW_WIDTH / 2.0f, 85.0f), WINS_NEEDED);

    // 过渡 / 横幅文字
    m_announceText = std::make_unique<sf::Text>(font);
    m_announceText->setCharacterSize(48);
    m_announceText->setFillColor(sf::Color::White);
}

MatchHud::~MatchHud() {
}

void MatchHud::update(const RoundManager& round, int roundsWonP1, int roundsWonP2,
                      bool matchOver, int matchWinner) {
    m_currentPhase = round.getPhase();

    // 比分（始终更新）
    m_scoreDisplay->update(roundsWonP1, roundsWonP2);

    // 计时器：仅 Fight 阶段
    if (m_currentPhase == RoundPhase::Fight) {
        m_timer->update(round.getTimeRemaining());
    }

    // 过渡 / 横幅文字
    std::string announce;
    if (matchOver && matchWinner >= 0) {
        announce = "PLAYER " + std::to_string(matchWinner + 1) + " WINS!";
    } else {
        switch (m_currentPhase) {
        case RoundPhase::Intro:
            announce = "ROUND " + std::to_string(round.getRoundNumber());
            break;
        case RoundPhase::Ready:
            announce = "FIGHT!";
            break;
        case RoundPhase::Fight:
            announce.clear();  // 倒计时由 Timer 显示
            break;
        case RoundPhase::RoundOver:
            if (round.getResult() == RoundResult::Draw) {
                announce = "DRAW";
            } else if (round.isTimeUp()) {
                announce = "TIME UP";
            } else {
                announce = "K.O.";
            }
            break;
        }
    }

    m_announceText->setString(announce);
    sf::FloatRect bounds = m_announceText->getLocalBounds();
    m_announceText->setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.0f,
                                           bounds.position.y + bounds.size.y / 2.0f));
    // 过渡文字位置：在计时器/比分下方，避免重叠
    m_announceText->setPosition(sf::Vector2f(WINDOW_WIDTH / 2.0f, 170.0f));
}

void MatchHud::render(sf::RenderWindow& window) {
    // 过渡 / 横幅（非空才画）
    if (m_announceText && m_announceText->getString().getSize() > 0) {
        window.draw(*m_announceText);
    }

    // 计时器（仅 Fight 阶段）
    if (m_currentPhase == RoundPhase::Fight) {
        m_timer->render(window);
    }

    // 比分（始终）
    m_scoreDisplay->render(window);
}

} // namespace SamuraiFight
