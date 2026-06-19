#pragma once

#include "CharacterState.h"
#include <string>
#include <utility>

namespace SamuraiFight {

/**
 * @file AttackState.h
 * @brief 攻击状态
 */

/**
 * @class AttackState
 * @brief 处理角色攻击动作的状态
 *
 * 攻击分为三个阶段：前摇(startup)、判定(active)、后摇(recovery)
 * 只有在判定帧期间才会激活攻击碰撞框
 */
class AttackState : public CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     * @param attackType 攻击类型 ("light", "medium", "heavy")
     */
    AttackState(Character* owner, const std::string& attackType);

    /**
     * @brief 析构函数
     */
    ~AttackState() override;

    /**
     * @brief 进入状态时调用
     */
    void onEnter() override;

    /**
     * @brief 退出状态时调用
     */
    void onExit() override;

    /**
     * @brief 更新状态逻辑
     *
     * @param deltaTime 时间增量
     */
    void update(float deltaTime) override;

    /**
     * @brief 获取攻击类型
     *
     * @return const std::string& 攻击类型
     */
    const std::string& getAttackType() const;

    /**
     * @brief 检查当前是否在判定帧期间
     *
     * @return true 在判定帧
     * @return false 不在判定帧
     */
    bool isInActiveFrames() const;

    /**
     * @brief 获取判定帧范围（开始帧，结束帧）
     *
     * @return std::pair<int, int> 判定帧范围
     */
    std::pair<int, int> getActiveFrameRange() const;

    /**
     * @brief 检查指定帧是否在判定帧期间
     *
     * @param frame 帧号
     * @return true 在判定帧
     * @return false 不在判定帧
     */
    bool isFrameInActiveFrames(int frame) const;

    /**
     * @brief 检查当前是否在可取消窗口
     *
     * @return true 可以取消
     * @return false 不能取消
     */
    bool canCancel() const;

    /**
     * @brief 检查攻击是否已完成
     *
     * @return true 已完成
     * @return false 未完成
     */
    bool isFinished() const;

    /**
     * @brief 获取攻击阶段 (0=前摇, 1=判定, 2=后摇)
     *
     * @return int 阶段
     */
    int getPhase() const;

    /**
     * @brief 获取当前攻击帧数
     *
     * @return int 帧数
     */
    int getCurrentAttackFrame() const;

    /**
     * @brief 获取最大命中次数（段数）
     *
     * @return int 段数
     */
    int getMaxHitCount() const;

    /**
     * @brief 获取当前已命中次数
     *
     * @return int 已命中次数
     */
    int getCurrentHitCount() const;

    /**
     * @brief 增加命中次数
     */
    void incrementHitCount();

    /**
     * @brief 检查是否还能命中
     *
     * @return true 还能命中
     * @return false 已达到最大命中次数
     */
    bool canHit() const;

private:
    std::string m_attackType;      ///< 攻击类型 (light, medium, heavy)
    int m_totalFrames;             ///< 攻击总帧数
    int m_startupEnd;              ///< 前摇结束帧
    int m_activeEnd;               ///< 判定结束帧
    bool m_hasHit;                 ///< 是否已经命中对手
    bool m_canCancel;              ///< 是否可取消
    int m_cancelWindowStart;       ///< 取消窗口开始帧
    int m_cancelWindowEnd;         ///< 取消窗口结束帧
    int m_maxHitCount;             ///< 最大命中次数（段数）
    int m_currentHitCount;         ///< 当前已命中次数
};

} // namespace SamuraiFight
