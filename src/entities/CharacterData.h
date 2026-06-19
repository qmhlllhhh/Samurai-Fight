#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <SFML/Graphics.hpp>

namespace SamuraiFight {

/**
 * @file CharacterData.h
 * @brief 角色数据结构定义
 */

/**
 * @struct AnimationData
 * @brief 动画数据
 */
struct AnimationData {
    std::string spriteSheet;    ///< 精灵表路径
    int frameCount;             ///< 帧数
    float frameDuration;        ///< 每帧持续时间（秒）
    bool loop;                  ///< 是否循环
    int frameWidth;             ///< 帧宽度
    int frameHeight;            ///< 帧高度
};

/**
 * @struct HitboxData
 * @brief 碰撞框数据
 */
struct HitboxData {
    float x;        ///< 相对X坐标
    float y;        ///< 相对Y坐标
    float width;    ///< 宽度
    float height;   ///< 高度
    std::string type; ///< 类型 (hurtbox/hitbox)
    std::vector<int> activeFrames; ///< 激活帧（仅对hitbox有效）
};

/**
 * @struct StatsData
 * @brief 角色属性数据
 */
struct StatsData {
    float maxHp;                ///< 最大生命值
    float maxStamina;           ///< 最大体力
    float moveSpeed;            ///< 移动速度
    float jumpHeight;           ///< 跳跃高度
    float staminaRecovery;      ///< 体力恢复速度
    float moveStaminaCost;      ///< 移动体力消耗
    float jumpStaminaCost;      ///< 跳跃体力消耗
};

/**
 * @struct CharacterData
 * @brief 角色完整数据
 */
struct CharacterData {
    std::string id;             ///< 角色ID
    std::string name;           ///< 角色名称
    std::string description;    ///< 角色描述

    StatsData stats;            ///< 属性数据

    std::unordered_map<std::string, AnimationData> animations;  ///< 动画数据
    std::unordered_map<std::string, std::vector<HitboxData>> hitboxes; ///< 碰撞框数据

    /**
     * @brief 默认构造函数
     */
    CharacterData() = default;

    /**
     * @brief 从JSON文件加载角色数据
     *
     * @param jsonPath JSON文件路径
     * @return true 加载成功
     * @return false 加载失败
     */
    bool loadFromJson(const std::string& jsonPath);
};

} // namespace SamuraiFight