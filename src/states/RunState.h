#pragma once

#include "CharacterState.h"

namespace SamuraiFight {

/**
 * @file RunState.h
 * @brief 跑步状态
 */

/**
 * @class RunState
 * @brief 角色跑步状态
 *
 * 跑步期间：
 * - 比走路快，比翻滚慢
 * - 消耗体力
 * - 有受击框
 * - 仅需按住方向键触发
 */
class RunState : public CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     */
    RunState(Character* owner);

    /**
     * @brief 析构函数
     */
    ~RunState() override;

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
    int m_direction;              ///< 跑步方向 (-1=左, 1=右)
    float m_runSpeed;             ///< 跑步速度
    float m_staminaCostPerFrame;  ///< 每帧体力消耗
};

} // namespace SamuraiFight
