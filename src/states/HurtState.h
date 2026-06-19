#pragma once

#include "CharacterState.h"

namespace SamuraiFight {

/**
 * @file HurtState.h
 * @brief 受击状态
 */

/**
 * @class HurtState
 * @brief 角色受到攻击后的硬直状态
 *
 * 受击状态期间角色无法进行任何操作
 */
class HurtState : public CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     * @param stunFrames 硬直帧数
     */
    HurtState(Character* owner, int stunFrames = 10);

    /**
     * @brief 析构函数
     */
    ~HurtState() override;

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
     * @brief 获取硬直帧数
     *
     * @return int 硬直帧数
     */
    int getStunFrames() const;

    /**
     * @brief 检查硬直是否结束
     *
     * @return true 硬直结束
     * @return false 硬直未结束
     */
    bool isStunFinished() const;

private:
    int m_stunFrames;      ///< 硬直帧数
    bool m_stunFinished;   ///< 硬直是否结束
};

} // namespace SamuraiFight
