#pragma once

#include <SFML/Audio.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace SamuraiFight {

/**
 * @file AudioManager.h
 * @brief 音频管理器 - 管理音效和背景音乐播放
 */

/**
 * @class AudioManager
 * @brief 单例音频管理器，处理所有游戏音频播放
 *
 * 功能：
 * - 音效播放：支持多音效同时播放，自动回收已播放完成的实例
 * - 背景音乐：支持淡入淡出切换，循环播放
 * - 音量控制：主音量、BGM音量、SFX音量独立控制
 * - 音效分组：按类别管理音效（combat, movement, ui, round）
 */
class AudioManager {
  public:
    /**
     * @brief 获取单例实例
     */
    static AudioManager &getInstance();

    /**
     * @brief 初始化音频管理器
     * @param configPath 配置文件路径
     * @return true 初始化成功
     */
    bool initialize(const std::string &configPath = "assets/config/game_config.json");

    /**
     * @brief 清理资源
     */
    void cleanup();

    // ========== 音效播放 ==========

    /**
     * @brief 播放音效
     * @param soundId 音效ID（对应 ResourceManager 中的 SoundBuffer ID）
     * @param volumeScale 音量缩放因子（0.0-1.0），默认1.0
     * @return true 播放成功
     */
    bool playSound(const std::string &soundId, float volumeScale = 1.0f);

    /**
     * @brief 停止所有音效
     */
    void stopAllSounds();

    /**
     * @brief 停止指定音效（停止所有该ID的播放实例）
     * @param soundId 音效ID
     */
    void stopSound(const std::string &soundId);

    // ========== 背景音乐 ==========

    /**
     * @brief 播放背景音乐
     * @param musicPath 音乐文件路径
     * @param loop 是否循环，默认true
     * @param fadeInDuration 淡入时长（秒），默认0.0
     * @return true 播放成功
     */
    bool playMusic(const std::string &musicPath, bool loop = true, float fadeInDuration = 0.0f);

    /**
     * @brief 停止背景音乐
     * @param fadeOutDuration 淡出时长（秒），默认0.0
     */
    void stopMusic(float fadeOutDuration = 0.0f);

    /**
     * @brief 暂停背景音乐
     */
    void pauseMusic();

    /**
     * @brief 恢复背景音乐
     */
    void resumeMusic();

    /**
     * @brief 检查背景音乐是否正在播放
     */
    bool isMusicPlaying() const;

    // ========== 音量控制 ==========

    /**
     * @brief 设置主音量（影响所有音频）
     * @param volume 音量值（0.0-1.0）
     */
    void setMasterVolume(float volume);

    /**
     * @brief 设置背景音乐音量
     * @param volume 音量值（0.0-1.0）
     */
    void setBGMVolume(float volume);

    /**
     * @brief 设置音效音量
     * @param volume 音量值（0.0-1.0）
     */
    void setSFXVolume(float volume);

    /**
     * @brief 获取主音量
     */
    float getMasterVolume() const;

    /**
     * @brief 获取背景音乐音量
     */
    float getBGMVolume() const;

    /**
     * @brief 获取音效音量
     */
    float getSFXVolume() const;

    // ========== 更新 ==========

    /**
     * @brief 每帧更新（处理淡入淡出、音效回收）
     * @param deltaTime 帧时间（秒）
     */
    void update(float deltaTime);

    // ========== 角色特定音效 ==========

    /**
     * @brief 播放角色特定音效
     * @param characterId 角色ID
     * @param soundType 音效类型（hurt, death, attack_light等）
     * @param volumeScale 音量缩放因子（0.0-1.0），默认1.0
     * @return true 播放成功
     */
    bool playCharacterSound(const std::string &characterId, const std::string &soundType, float volumeScale = 1.0f);

    // ========== 场景背景音乐 ==========

    /**
     * @brief 播放场景背景音乐
     * @param sceneType 场景类型（main_menu, character_select, battle, victory等）
     * @param fadeInDuration 淡入时长（秒），默认1.0
     * @return true 播放成功
     */
    bool playSceneMusic(const std::string &sceneType, float fadeInDuration = 1.0f);

  private:
    AudioManager();
    ~AudioManager();

    // 禁用拷贝和移动
    AudioManager(const AudioManager &) = delete;
    AudioManager &operator=(const AudioManager &) = delete;
    AudioManager(AudioManager &&) = delete;
    AudioManager &operator=(AudioManager &&) = delete;

    /**
     * @brief 从配置文件加载音量设置
     */
    bool loadVolumeFromConfig(const std::string &configPath);

    /**
     * @brief 应用音量到背景音乐
     */
    void applyMusicVolume();

    /**
     * @brief 应用音量到所有活跃音效
     */
    void applySoundVolumes();

    /**
     * @brief 清理已停止的音效实例
     */
    void cleanupStoppedSounds();

    // ========== 成员变量 ==========

    float m_masterVolume; ///< 主音量（0.0-1.0）
    float m_bgmVolume;    ///< 背景音乐音量（0.0-1.0）
    float m_sfxVolume;    ///< 音效音量（0.0-1.0）

    std::unique_ptr<sf::Music> m_currentMusic; ///< 当前背景音乐
    std::string m_currentMusicPath;            ///< 当前音乐路径
    bool m_musicFading;                        ///< 音乐是否正在淡入淡出
    float m_musicFadeTimer;                    ///< 淡入淡出计时器
    float m_musicFadeDuration;                 ///< 淡入淡出总时长
    float m_musicFadeStartVolume;              ///< 淡入淡出起始音量
    float m_musicFadeTargetVolume;             ///< 淡入淡出目标音量
    bool m_musicFadeOutAndStop;                ///< 淡出后是否停止

    static constexpr size_t MAX_SOUND_INSTANCES = 32;                               ///< 最大同时播放音效数
    std::vector<std::pair<std::string, std::unique_ptr<sf::Sound>>> m_activeSounds; ///< 活跃音效列表

    // 角色音效映射：characterId -> (soundType -> soundId)
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_characterSounds;

    // 场景音乐映射：sceneType -> musicPath
    std::unordered_map<std::string, std::string> m_sceneMusicPaths;
};

} // namespace SamuraiFight
