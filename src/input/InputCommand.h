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
    MoveDown,       ///< 向下（用于移动触发条件）
    Jump,           ///< 跳跃
    AttackLight,    ///< 轻攻击
    AttackMedium,   ///< 中攻击
    AttackHeavy,    ///< 重攻击
    AttackSpecial,  ///< 特殊技
    Block,          ///< 防御
    Roll            ///< 翻滚
};

/**
 * @struct InputState
 * @brief 输入状态
 *
 * 存储所有输入动作的当前状态
 */
struct InputState {
    bool moveLeft = false;      ///< 走路向左（S + 左）
    bool moveRight = false;     ///< 走路向右（S + 右）
    bool runLeft = false;       ///< 跑步向左（仅左）
    bool runRight = false;      ///< 跑步向右（仅右）
    bool jump = false;
    bool attackLight = false;
    bool attackMedium = false;
    bool attackHeavy = false;
    bool attackSpecial = false;
    bool block = false;
    bool roll = false;

    /**
     * @brief 重置所有输入
     */
    void reset() {
        moveLeft = false;
        moveRight = false;
        runLeft = false;
        runRight = false;
        jump = false;
        attackLight = false;
        attackMedium = false;
        attackHeavy = false;
        attackSpecial = false;
        block = false;
        roll = false;
    }
};

} // namespace SamuraiFight