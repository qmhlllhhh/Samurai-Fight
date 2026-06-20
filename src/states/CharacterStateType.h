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
    Idle,           ///< 站立
    Walk,           ///< 走路
    Run,            ///< 跑步
    Jump,           ///< 跳跃
    AttackLight,    ///< 轻攻击
    AttackMedium,   ///< 中攻击
    AttackHeavy,    ///< 重攻击
    Block,          ///< 防御
    Hurt,           ///< 受击
    Roll,           ///< 翻滚
    Dead,           ///< 死亡
    Victory         ///< 胜利
};

} // namespace SamuraiFight