#pragma once

namespace SamuraiFight {

// 前向声明
class Character;

/**
 * @file RoundManager.h
 * @brief 回合管理器
 */

/**
 * @enum RoundPhase
 * @brief 回合阶段
 */
enum class RoundPhase {
    Intro,      ///< 显示 "ROUND n"
    Ready,      ///< 显示 "FIGHT!"，角色已就位但锁定输入
    Fight,      ///< 正式对战，倒计时进行
    RoundOver   ///< 显示 "K.O." / "TIME UP"，等待转场
};

/**
 * @enum RoundResult
 * @brief 回合结果
 */
enum class RoundResult {
    None,
    Player1Win,
    Player2Win,
    Draw        ///< 平局，双方各 0 胜
};

/**
 * @class RoundManager
 * @brief 管理单个回合的生命周期与胜负判定
 *
 * 驱动 Intro → Ready → Fight → RoundOver 的阶段切换，
 * 在 Fight 阶段进行倒计时与胜负判定。不拥有 Character，
 * 仅持有指针引用。
 */
class RoundManager {
public:
    /**
     * @brief 构造函数
     *
     * @param p1 玩家1角色
     * @param p2 玩家2角色
     * @param roundTime 回合时长（秒）
     * @param roundNumber 回合序号（从 1 开始）
     */
    RoundManager(Character* p1, Character* p2, int roundTime, int roundNumber);

    /**
     * @brief 析构函数
     */
    ~RoundManager();

    /**
     * @brief 开始回合（重置角色 → 进入 Intro）
     */
    void start();

    /**
     * @brief 每帧驱动回合逻辑
     *
     * @param deltaTime 固定时间步长
     */
    void update(float deltaTime);

    /**
     * @brief 当前是否锁定玩家输入
     *
     * @return true 非 Fight 阶段，锁定输入
     * @return false Fight 阶段，允许输入
     */
    bool isInputLocked() const;

    /**
     * @brief 回合是否已结束（RoundOver 停留期满）
     *
     * @return true 已结束，可进入下一回合
     * @return false 进行中
     */
    bool isFinished() const;

    /**
     * @brief 获取回合结果
     *
     * @return RoundResult 回合结果（None 表示尚未判定）
     */
    RoundResult getResult() const;

    /**
     * @brief 回合是否因时间到而结束
     *
     * @return true 时间到（应显示 TIME UP）
     * @return false 击倒（应显示 K.O.）
     */
    bool isTimeUp() const;

    /**
     * @brief 获取剩余时间（显示用，向上取整秒）
     *
     * @return int 剩余秒数
     */
    int getTimeRemaining() const;

    /**
     * @brief 获取当前阶段
     *
     * @return RoundPhase 当前阶段
     */
    RoundPhase getPhase() const;

    /**
     * @brief 获取回合序号
     *
     * @return int 回合序号
     */
    int getRoundNumber() const;

private:
    /**
     * @brief 切换阶段并重置阶段计时器
     *
     * @param phase 新阶段
     */
    void setPhase(RoundPhase phase);

    /**
     * @brief 检查回合结束条件（HP 归零或时间到）
     */
    void checkRoundEnd();

    /**
     * @brief 重置双方角色到回合起始状态
     */
    void resetCharacters();

    Character* m_players[2];   ///< 双方角色（不拥有）
    int m_roundTime;           ///< 回合时长（秒）
    int m_roundNumber;         ///< 回合序号

    RoundPhase m_phase;        ///< 当前阶段
    float m_phaseTimer;        ///< 当前阶段已持续时间（秒）
    float m_timeRemaining;     ///< Fight 阶段剩余时间（秒）
    RoundResult m_result;      ///< 回合结果
    bool m_timeUp;             ///< 是否因时间到结束（区分 K.O. / TIME UP）
};

} // namespace SamuraiFight
