#pragma once

#include "CharacterState.h"

namespace SamuraiFight {

/**
 * @file IdleState.h
 * @brief 站立状态
 */

/**
 * @class IdleState
 * @brief 角色站立状态
 *
 * 角色静止不动时的状态，恢复体力
 */
class IdleState : public CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     */
    IdleState(Character* owner);

    /**
     * @brief 析构函数
     */
    ~IdleState() override;

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
};

} // namespace SamuraiFight