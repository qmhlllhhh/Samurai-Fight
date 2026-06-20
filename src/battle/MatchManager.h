#pragma once

#include <memory>

namespace SamuraiFight {

// 前向声明
class Character;
class RoundManager;

/**
 * @file MatchManager.h
 * @brief 比赛管理器
 */

/**
 * @enum MatchState
 * @brief 比赛状态
 */
enum class MatchState {
    InProgress,  ///< 进行中
    MatchOver    ///< 已结束
};

/**
 * @class MatchManager
 * @brief 管理整局比赛（三局两胜），调度回合切换与最终胜负
 *
 * 持有当前 RoundManager，负责回合计分与流转：
 * 每个回合结束（isFinished）时按结果计分（平局不加分），
 * 任一方达到 winsNeeded 即比赛结束；否则自动开始下一回合。
 * 不拥有 Character，仅持有指针引用。
 */
class MatchManager {
public:
    /**
     * @brief 构造函数（构造后自动开始第一回合）
     *
     * @param p1 玩家1角色
     * @param p2 玩家2角色
     * @param roundTime 每回合时长（秒）
     * @param winsNeeded 获胜所需局数（如 2 = 三局两胜）
     */
    MatchManager(Character* p1, Character* p2, int roundTime, int winsNeeded);

    /**
     * @brief 析构函数
     */
    ~MatchManager();

    /**
     * @brief 每帧驱动比赛逻辑（驱动当前回合并处理回合结束）
     *
     * @param deltaTime 固定时间步长
     */
    void update(float deltaTime);

    /**
     * @brief 开始下一回合（创建新 RoundManager 并 start）
     */
    void startNextRound();

    /**
     * @brief 获取当前回合管理器
     *
     * @return RoundManager* 当前回合（比赛结束后仍指向最后一回合）
     */
    RoundManager* getCurrentRound() const;

    /**
     * @brief 获取指定玩家的胜场数
     *
     * @param playerIndex 玩家索引（0 或 1）
     * @return int 胜场数
     */
    int getRoundsWon(int playerIndex) const;

    /**
     * @brief 获取当前回合序号
     *
     * @return int 回合序号（从 1 开始）
     */
    int getRoundNumber() const;

    /**
     * @brief 获取比赛状态
     *
     * @return MatchState 比赛状态
     */
    MatchState getMatchState() const;

    /**
     * @brief 获取比赛胜者
     *
     * @return int 胜者索引（0/1），-1 表示未结束
     */
    int getMatchWinner() const;

private:
    Character* m_players[2];                       ///< 双方角色（不拥有）
    int m_roundTime;                               ///< 每回合时长（秒）
    int m_winsNeeded;                              ///< 获胜所需局数

    std::unique_ptr<RoundManager> m_currentRound;  ///< 当前回合
    int m_roundsWon[2];                            ///< 双方胜场
    int m_roundNumber;                             ///< 当前回合序号
    MatchState m_matchState;                       ///< 比赛状态
    int m_matchWinner;                             ///< 比赛胜者（-1=未决）
};

} // namespace SamuraiFight
