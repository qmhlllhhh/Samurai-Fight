#pragma once

namespace SamuraiFight {

/**
 * @file InputCommand.h
 * @brief 输入命令定义
 */

/**
 * @enum InputAction
 * @brief 输入动作类型
 */
enum class InputAction {
    MoveLeft,       ///< 向左移动
    MoveRight,      ///< 向右移动
    Jump,           ///< 跳跃
    Crouch,         ///< 下蹲
    AttackLight,    ///< 轻攻击
    AttackMedium,   ///< 中攻击
    AttackHeavy,    ///< 重攻击
    AttackSpecial,  ///< 特殊技
    Block           ///< 防御
};

/**
 * @struct InputState
 * @brief 输入状态
 *
 * 存储所有输入动作的当前状态
 */
struct InputState {
    bool moveLeft = false;
    bool moveRight = false;
    bool jump = false;
    bool crouch = false;
    bool attackLight = false;
    bool attackMedium = false;
    bool attackHeavy = false;
    bool attackSpecial = false;
    bool block = false;

    /**
     * @brief 重置所有输入
     */
    void reset() {
        moveLeft = false;
        moveRight = false;
        jump = false;
        crouch = false;
        attackLight = false;
        attackMedium = false;
        attackHeavy = false;
        attackSpecial = false;
        block = false;
    }
};

} // namespace SamuraiFight