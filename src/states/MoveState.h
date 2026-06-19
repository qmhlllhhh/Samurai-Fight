#pragma once

#include "CharacterState.h"

namespace SamuraiFight {

/**
 * @file MoveState.h
 * @brief 移动状态
 */

/**
 * @class MoveState
 * @brief 角色移动状态
 *
 * 角色左右移动时的状态，消耗体力
 */
class MoveState : public CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     */
    MoveState(Character* owner);

    /**
     * @brief 析构函数
     */
    ~MoveState() override;

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
    int m_direction; ///< 移动方向 (-1=左, 1=右)
};

} // namespace SamuraiFight