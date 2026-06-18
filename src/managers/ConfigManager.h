#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include "../utils/JsonLoader.h"

namespace SamuraiFight {

/**
 * @file ConfigManager.h
 * @brief 配置管理器
 */

/**
 * @class ConfigManager
 * @brief 加载和管理游戏配置文件
 *
 * 管理游戏配置（窗口、游戏机制、音频设置）和键位配置
 */
class ConfigManager {
public:
    /**
     * @brief 构造函数
     */
    ConfigManager();

    /**
     * @brief 析构函数
     */
    ~ConfigManager();

    /**
     * @brief 初始化配置管理器
     *
     * 加载默认配置文件
     *
     * @return true 初始化成功
     * @return false 初始化失败
     */
    bool initialize();

    /**
     * @brief 加载游戏配置文件
     *
     * @param path 配置文件路径
     * @return true 加载成功
     * @return false 加载失败
     */
    bool loadGameConfig(const std::string& path);

    /**
     * @brief 加载键位配置文件
     *
     * @param path 配置文件路径
     * @return true 加载成功
     * @return false 加载失败
     */
    bool loadKeyBindings(const std::string& path);

    /**
     * @brief 获取游戏配置
     *
     * @return const nlohmann::json& 游戏配置JSON对象
     */
    const nlohmann::json& getGameConfig() const;

    /**
     * @brief 获取键位配置
     *
     * @return const nlohmann::json& 键位配置JSON对象
     */
    const nlohmann::json& getKeyBindings() const;

    /**
     * @brief 获取窗口宽度
     *
     * @return int 窗口宽度
     */
    int getWindowWidth() const;

    /**
     * @brief 获取窗口高度
     *
     * @return int 窗口高度
     */
    int getWindowHeight() const;

    /**
     * @brief 获取窗口标题
     *
     * @return std::string 窗口标题
     */
    std::string getWindowTitle() const;

    /**
     * @brief 获取是否全屏
     *
     * @return true 全屏
     * @return false 窗口模式
     */
    bool isFullscreen() const;

    /**
     * @brief 获取是否启用VSync
     *
     * @return true 启用VSync
     * @return false 禁用VSync
     */
    bool isVSyncEnabled() const;

    /**
     * @brief 获取目标帧率
     *
     * @return int 目标帧率
     */
    int getTargetFPS() const;

    /**
     * @brief 获取回合时间
     *
     * @return int 回合时间（秒）
     */
    int getRoundTime() const;

    /**
     * @brief 获取获胜所需局数
     *
     * @return int 获胜所需局数
     */
    int getWinsNeeded() const;

    /**
     * @brief 获取主音量
     *
     * @return float 主音量 (0.0 - 1.0)
     */
    float getMasterVolume() const;

    /**
     * @brief 获取BGM音量
     *
     * @return float BGM音量 (0.0 - 1.0)
     */
    float getBGMVolume() const;

    /**
     * @brief 获取音效音量
     *
     * @return float 音效音量 (0.0 - 1.0)
     */
    float getSFXVolume() const;

private:
    nlohmann::json m_gameConfig;    ///< 游戏配置
    nlohmann::json m_keyBindings;   ///< 键位配置
};

} // namespace SamuraiFight