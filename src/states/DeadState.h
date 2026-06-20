#pragma once

#include "CharacterState.h"

namespace SamuraiFight {

/**
 * @file DeadState.h
 * @brief 死亡状态
 */

/**
 * @class DeadState
 * @brief 角色死亡状态
 *
 * 死亡状态期间角色无法进行任何操作
 * 播放死亡动画后停留在最后一帧
 */
class DeadState : public CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     */
    DeadState(Character* owner);

    /**
     * @brief 析构函数
     */
    ~DeadState() override;

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
     * @brief 检查死亡动画是否播放完成
     *
     * @return true 动画播放完成
     * @return false 动画仍在播放
     */
    bool isAnimationFinished() const;

private:
    bool m_animationFinished;  ///< 动画是否播放完成
};

} // namespace SamuraiFight
