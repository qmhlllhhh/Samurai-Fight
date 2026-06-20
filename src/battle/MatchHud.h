#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "RoundManager.h"

namespace SamuraiFight {

/**
 * @file MatchHud.h
 * @brief 比赛HUD
 */

/**
 * @class MatchHud
 * @brief 比赛信息显示组件
 *
 * 显示回合过渡文字（ROUND n / FIGHT! / 倒计时 / K.O. / TIME UP / DRAW）
 * 以及双方比分。
 */
class MatchHud {
public:
    /**
     * @brief 构造函数
     */
    MatchHud();

    /**
     * @brief 析构函数
     */
    ~MatchHud();

    /**
     * @brief 更新HUD内容
     *
     * @param round 当前回合管理器
     * @param roundsWonP1 玩家1胜场
     * @param roundsWonP2 玩家2胜场
     * @param matchOver 比赛是否已结束
     * @param matchWinner 比赛胜者索引（0/1，-1=未决）
     */
    void update(const RoundManager& round, int roundsWonP1, int roundsWonP2,
                bool matchOver, int matchWinner);

    /**
     * @brief 渲染HUD
     *
     * @param window 游戏窗口
     */
    void render(sf::RenderWindow& window);

private:
    std::unique_ptr<sf::Text> m_announceText;  ///< 过渡文字/倒计时/结果
    std::unique_ptr<sf::Text> m_scoreText;     ///< 比分
};

} // namespace SamuraiFight
