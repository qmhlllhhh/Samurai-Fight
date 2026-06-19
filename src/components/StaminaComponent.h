#pragma once

#include <SFML/Graphics.hpp>

namespace SamuraiFight {

/**
 * @file StaminaComponent.h
 * @brief 体力值组件
 */

/**
 * @class StaminaComponent
 * @brief 管理角色的体力值
 *
 * 包含当前体力值、最大体力值、消耗和恢复等功能
 */
class StaminaComponent {
public:
    /**
     * @brief 构造函数
     *
     * @param maxStamina 最大体力值
     */
    explicit StaminaComponent(float maxStamina = 100.0f);

    /**
     * @brief 析构函数
     */
    ~StaminaComponent();

    /**
     * @brief 消耗体力
     *
     * @param amount 消耗量
     * @return float 实际消耗的量
     */
    float consume(float amount);

    /**
     * @brief 恢复体力
     *
     * @param amount 恢复量
     * @return float 实际恢复的量
     */
    float recover(float amount);

    /**
     * @brief 更新体力值（每帧调用）
     *
     * @param isMoving 是否在移动
     * @param isJumping 是否在跳跃
     * @param isStanding 是否站立
     * @param moveCostPerFrame 移动每帧消耗
     * @param recoveryPerFrame 站立每帧恢复
     * @param jumpCost 跳跃消耗
     */
    void update(bool isMoving, bool isJumping, bool isStanding,
                float moveCostPerFrame, float recoveryPerFrame, float jumpCost);

    /**
     * @brief 获取当前体力值
     *
     * @return float 当前体力值
     */
    float getCurrentStamina() const;

    /**
     * @brief 获取最大体力值
     *
     * @return float 最大体力值
     */
    float getMaxStamina() const;

    /**
     * @brief 获取体力值百分比
     *
     * @return float 体力值百分比 (0.0 - 1.0)
     */
    float getStaminaPercent() const;

    /**
     * @brief 检查是否体力耗尽
     *
     * @return true 体力耗尽
     * @return false 还有体力
     */
    bool isExhausted() const;

    /**
     * @brief 检查是否有足够体力进入状态
     *
     * @param amount 需要的体力值
     * @return true 有足够体力
     * @return false 体力不足
     */
    bool hasEnoughStamina(float amount) const;

    /**
     * @brief 检查体力值是否低于阈值（20%）
     *
     * @return true 低于阈值
     * @return false 高于或等于阈值
     */
    bool isBelowThreshold() const;

    /**
     * @brief 获取体力阈值（20%）
     *
     * @return float 阈值
     */
    float getThreshold() const;

    /**
     * @brief 检查是否可以进入需要体力的状态（跳跃、防御）
     *
     * @return true 可以进入
     * @return false 体力不足
     */
    bool canEnterStaminaState() const;

    /**
     * @brief 重置体力值到最大值
     */
    void reset();

    /**
     * @brief 设置最大体力值
     *
     * @param maxStamina 新的最大体力值
     */
    void setMaxStamina(float maxStamina);

private:
    float m_currentStamina; ///< 当前体力值
    float m_maxStamina;     ///< 最大体力值
    float m_threshold;      ///< 体力阈值（用于判断是否可以进入需要体力的状态）
    bool m_wasJumping;      ///< 上一帧是否跳跃（用于检测跳跃开始）
};

} // namespace SamuraiFight
