#pragma once

#include "CharacterState.h"

namespace SamuraiFight {

/**
 * @file VictoryState.h
 * @brief 胜利状态
 */

/**
 * @class VictoryState
 * @brief 角色胜利状态
 *
 * 比赛获胜后进入此状态，播放胜利动画（若有专用素材），
 * 期间不接受任何输入，保持胜利姿态直到比赛结束切场景。
 */
class VictoryState : public CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     */
    explicit VictoryState(Character* owner);

    /**
     * @brief 析构函数
     */
    ~VictoryState() override;

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
