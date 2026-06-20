#pragma once

#include <SFML/Graphics.hpp>

namespace SamuraiFight {

/**
 * @file Constants.h
 * @brief 游戏全局常量定义
 */

// 帧率相关
constexpr int TARGET_FPS = 60;
constexpr float FRAME_TIME = 1.0f / static_cast<float>(TARGET_FPS);

// 窗口相关
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr char WINDOW_TITLE[] = "Samurai Fight";

// 游戏机制相关
constexpr int MAX_HEALTH = 100;
constexpr int MAX_STAMINA = 100;
constexpr int ROUND_TIME = 60; // 秒
constexpr int WINS_NEEDED = 2; // 三局两胜

// 输入缓冲
constexpr int INPUT_BUFFER_SIZE = 12; // 12帧缓冲窗口

// 体力消耗
constexpr float STAMINA_RECOVERY_RATE = 0.5f;    // 站立时每帧恢复
constexpr float MOVE_STAMINA_RECOVERY = 0.1f;    // 移动每帧恢复
constexpr float JUMP_STAMINA_COST = 10.0f;       // 跳跃消耗
constexpr float BLOCK_STAMINA_MULTIPLIER = 0.5f; // 防御时体力消耗倍率

// 防御减免
constexpr float BLOCK_DAMAGE_REDUCTION = 0.5f; // 防御时伤害减免50%

// 角色尺寸
constexpr int CHARACTER_WIDTH = 128;
constexpr int CHARACTER_HEIGHT = 128;
constexpr float CHARACTER_TARGET_SIZE = 360.0f;

// 物理相关
constexpr float GRAVITY = 1500.0f;            // 重力加速度 (像素/秒²)
constexpr float GROUND_LEVEL = 600.0f;        // 地面Y坐标
constexpr float DEFAULT_MOVE_SPEED = 300.0f;  // 默认移动速度
constexpr float DEFAULT_JUMP_HEIGHT = 900.0f; // 默认跳跃高度 (空中1.2秒)

// 场景背景数量
constexpr int BACKGROUND_COUNT = 3;

// 默认音量
constexpr float DEFAULT_MASTER_VOLUME = 1.0f;
constexpr float DEFAULT_BGM_VOLUME = 0.8f;
constexpr float DEFAULT_SFX_VOLUME = 1.0f;

// 性能目标
constexpr size_t MAX_MEMORY_MB = 500;
constexpr float MAX_STARTUP_TIME_SEC = 3.0f;
constexpr float MAX_SCENE_TRANSITION_TIME_SEC = 1.0f;

// 调试相关
#ifdef DEBUG
constexpr bool DEBUG_MODE = true;
#else
constexpr bool DEBUG_MODE = false;
#endif

} // namespace SamuraiFight