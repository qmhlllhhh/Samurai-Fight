#pragma once

#include "CharacterState.h"

namespace SamuraiFight {

/**
 * @file RollState.h
 * @brief 翻滚状态
 */

/**
 * @class RollState
 * @brief 角色翻滚状态
 *
 * 翻滚期间：
 * - 快速移动
 * - 无敌状态
 * - 方向由进入时的朝向决定
 * - 一定帧数后自动结束
 */
class RollState : public CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     */
    RollState(Character* owner);

    /**
     * @brief 析构函数
     */
    ~RollState() override;

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
     * @brief 获取翻滚总帧数
     *
     * @return int 帧数
     */
    int getTotalFrames() const;

private:
    int m_totalFrames;      ///< 翻滚总帧数
    float m_rollSpeed;      ///< 翻滚速度
    bool m_facingRight;     ///< 翻滚方向（进入时的朝向）
};

} // namespace SamuraiFight
