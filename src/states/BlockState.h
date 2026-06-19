#pragma once

#include "CharacterState.h"

namespace SamuraiFight {

/**
 * @file BlockState.h
 * @brief 防御状态
 */

/**
 * @class BlockState
 * @brief 角色防御状态
 *
 * 防御期间：
 * - 移动速度变慢
 * - 不能转换面朝方向
 * - 持续消耗体力值
 * - 受到攻击时对手陷入硬直
 */
class BlockState : public CharacterState {
  public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     */
    BlockState(Character *owner);

    /**
     * @brief 析构函数
     */
    ~BlockState() override;

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
     * @brief 检查是否有足够体力防御
     *
     * @return true 有足够体力
     * @return false 体力不足
     */
    bool hasEnoughStamina() const;

    /**
     * @brief 被攻击命中（防御成功）
     */
    void onBlockHit();

    /**
     * @brief 检查是否被攻击打破防御
     *
     * @return true 被打破
     * @return false 未被打破
     */
    bool isSuccess() const;

  private:
    bool m_blockSuccess; ///< 是否被攻击打破防御
};

} // namespace SamuraiFight
