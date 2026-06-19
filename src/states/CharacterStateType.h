#pragma once

namespace SamuraiFight {

/**
 * @file CharacterStateType.h
 * @brief 角色状态类型枚举
 */

/**
 * @enum CharacterStateType
 * @brief 角色状态类型
 */
enum class CharacterStateType {
    Idle,       ///< 站立
    Move,       ///< 移动
    Jump,       ///< 跳跃
    Crouch,     ///< 下蹲
    Attack,     ///< 攻击（阶段3）
    Block,      ///< 防御（阶段3）
    Hurt,       ///< 受击（阶段3）
    Victory     ///< 胜利（阶段4）
};

} // namespace SamuraiFight