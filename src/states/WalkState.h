#pragma once

#include "CharacterState.h"

namespace SamuraiFight {

/**
 * @file WalkState.h
 * @brief 走路状态
 */

/**
 * @class WalkState
 * @brief 角色走路状态
 *
 * 角色左右走路时的状态，需要按住S+方向键
 */
class WalkState : public CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     */
    WalkState(Character* owner);

    /**
     * @brief 析构函数
     */
    ~WalkState() override;

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