#pragma once

#include "CharacterState.h"

namespace SamuraiFight {

/**
 * @file CrouchState.h
 * @brief 下蹲状态
 */

/**
 * @class CrouchState
 * @brief 角色下蹲状态
 *
 * 角色下蹲时的状态，降低受击判定
 */
class CrouchState : public CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     */
    CrouchState(Character* owner);

    /**
     * @brief 析构函数
     */
    ~CrouchState() override;

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