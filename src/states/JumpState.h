#pragma once

#include "CharacterState.h"

namespace SamuraiFight {

/**
 * @file JumpState.h
 * @brief 跳跃状态
 */

/**
 * @class JumpState
 * @brief 角色跳跃状态
 *
 * 角色跳跃时的状态，消耗体力
 */
class JumpState : public CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     */
    JumpState(Character* owner);

    /**
     * @brief 析构函数
     */
    ~JumpState() override;

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

private:
    bool m_isFalling; ///< 是否在下落
};

} // namespace SamuraiFight