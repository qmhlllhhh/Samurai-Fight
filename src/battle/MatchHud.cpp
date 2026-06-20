#include "MatchHud.h"
#include "../managers/ResourceManager.h"
#include "../core/Constants.h"
#include <string>

namespace SamuraiFight {

MatchHud::MatchHud() {
    const sf::Font& font = ResourceManager::getInstance().getDefaultFont();

    m_announceText = std::make_unique<sf::Text>(font);
    m_announceText->setCharacterSize(48);
    m_announceText->setFillColor(sf::Color::White);

    m_scoreText = std::make_unique<sf::Text>(font);
    m_scoreText->setCharacterSize(36);
    m_scoreText->setFillColor(sf::Color::Yellow);
}

MatchHud::~MatchHud() {
}

void MatchHud::update(const RoundManager& round, int roundsWonP1, int roundsWonP2,
                      bool matchOver, int matchWinner) {
    // 过渡/倒计时/结果文字
    std::string announce;
    if (matchOver && matchWinner >= 0) {
        // 比赛结束：显示胜负横幅（胜者无专用 victory 动画，用文字明确结果）
        announce = "PLAYER " + std::to_string(matchWinner + 1) + " WINS!";
    } else {
        switch (round.getPhase()) {
        case RoundPhase::Intro:
            announce = "ROUND " + std::to_string(round.getRoundNumber());
            break;
        case RoundPhase::Ready:
            announce = "FIGHT!";
            break;
        case RoundPhase::Fight:
            announce = std::to_string(round.getTimeRemaining());
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
    m_announceText->setPosition(sf::Vector2f(WINDOW_WIDTH / 2.0f, 90.0f));

    // 比分
    m_scoreText->setString(std::to_string(roundsWonP1) + " - " + std::to_string(roundsWonP2));
    sf::FloatRect scoreBounds = m_scoreText->getLocalBounds();
    m_scoreText->setOrigin(sf::Vector2f(scoreBounds.position.x + scoreBounds.size.x / 2.0f,
                                        scoreBounds.position.y + scoreBounds.size.y / 2.0f));
    m_scoreText->setPosition(sf::Vector2f(WINDOW_WIDTH / 2.0f, 150.0f));
}

void MatchHud::render(sf::RenderWindow& window) {
    if (m_announceText) {
        window.draw(*m_announceText);
    }
    if (m_scoreText) {
        window.draw(*m_scoreText);
    }
}

} // namespace SamuraiFight
