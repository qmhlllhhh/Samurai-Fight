#include "AudioManager.h"
#include "../utils/JsonLoader.h"
#include "ResourceManager.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

namespace SamuraiFight {

AudioManager &AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

AudioManager::AudioManager()
    : m_masterVolume(0.8f), m_bgmVolume(0.8f), m_sfxVolume(1.0f), m_currentMusic(std::make_unique<sf::Music>()), m_musicFading(false), m_musicFadeTimer(0.0f), m_musicFadeDuration(0.0f), m_musicFadeStartVolume(0.0f), m_musicFadeTargetVolume(0.0f), m_musicFadeOutAndStop(false) {
}

AudioManager::~AudioManager() {
    cleanup();
}

bool AudioManager::initialize(const std::string &configPath) {
    // 从配置文件加载音量设置
    if (!loadVolumeFromConfig(configPath)) {
        std::cerr << "AudioManager: Warning: Failed to load volume from config, using defaults" << std::endl;
    }

    std::cout << "AudioManager: Initialized with volumes - Master: " << m_masterVolume
              << ", BGM: " << m_bgmVolume << ", SFX: " << m_sfxVolume << std::endl;
    return true;
}

void AudioManager::cleanup() {
    stopAllSounds();
    stopMusic(0.0f);
    std::cout << "AudioManager: Cleaned up" << std::endl;
}

// ========== 音效播放 ==========

bool AudioManager::playSound(const std::string &soundId, float volumeScale) {
    auto &rm = ResourceManager::getInstance();

    // 检查音效是否存在
    if (!rm.hasSoundBuffer(soundId)) {
        std::cerr << "AudioManager: Sound buffer '" << soundId << "' not found" << std::endl;
        return false;
    }

    // 清理已停止的音效
    cleanupStoppedSounds();

    // 检查是否超过最大实例数
    if (m_activeSounds.size() >= MAX_SOUND_INSTANCES) {
        std::cerr << "AudioManager: Warning: Max sound instances reached, stopping oldest" << std::endl;
        // 移除最早的已停止音效，如果没有则强制停止第一个
        if (!m_activeSounds.empty()) {
            m_activeSounds.erase(m_activeSounds.begin());
        }
    }

    // 创建新的音效实例
    auto sound = std::make_unique<sf::Sound>(rm.getSoundBuffer(soundId));

    // 计算最终音量
    float finalVolume = m_masterVolume * m_sfxVolume * volumeScale * 100.0f;
    sound->setVolume(std::clamp(finalVolume, 0.0f, 100.0f));
    sound->play();

    m_activeSounds.emplace_back(soundId, std::move(sound));
    return true;
}

void AudioManager::stopAllSounds() {
    for (auto &[id, sound] : m_activeSounds) {
        if (sound && sound->getStatus() == sf::Sound::Status::Playing) {
            sound->stop();
        }
    }
    m_activeSounds.clear();
}

void AudioManager::stopSound(const std::string &soundId) {
    for (auto it = m_activeSounds.begin(); it != m_activeSounds.end();) {
        if (it->first == soundId) {
            if (it->second && it->second->getStatus() == sf::Sound::Status::Playing) {
                it->second->stop();
            }
            it = m_activeSounds.erase(it);
        } else {
            ++it;
        }
    }
}

// ========== 背景音乐 ==========

bool AudioManager::playMusic(const std::string &musicPath, bool loop, float fadeInDuration) {
    // 如果正在播放同一首音乐，不做任何事
    if (m_currentMusicPath == musicPath && isMusicPlaying()) {
        return true;
    }

    // 停止当前音乐（无淡出）
    if (m_currentMusic && m_currentMusic->getStatus() == sf::Music::Status::Playing) {
        m_currentMusic->stop();
    }

    // 加载新音乐
    if (!m_currentMusic->openFromFile(musicPath)) {
        std::cerr << "AudioManager: Failed to load music from " << musicPath << std::endl;
        return false;
    }

    m_currentMusicPath = musicPath;
    m_currentMusic->setLooping(loop);

    // 设置淡入
    if (fadeInDuration > 0.0f) {
        m_musicFading = true;
        m_musicFadeTimer = 0.0f;
        m_musicFadeDuration = fadeInDuration;
        m_musicFadeStartVolume = 0.0f;
        m_musicFadeTargetVolume = m_masterVolume * m_bgmVolume;
        m_musicFadeOutAndStop = false;
        m_currentMusic->setVolume(0.0f);
    } else {
        applyMusicVolume();
        m_musicFading = false;
    }

    m_currentMusic->play();
    std::cout << "AudioManager: Playing music: " << musicPath << std::endl;
    return true;
}

void AudioManager::stopMusic(float fadeOutDuration) {
    if (!m_currentMusic || m_currentMusic->getStatus() != sf::Music::Status::Playing) {
        return;
    }

    if (fadeOutDuration > 0.0f) {
        // 开始淡出
        m_musicFading = true;
        m_musicFadeTimer = 0.0f;
        m_musicFadeDuration = fadeOutDuration;
        m_musicFadeStartVolume = m_currentMusic->getVolume() / 100.0f;
        m_musicFadeTargetVolume = 0.0f;
        m_musicFadeOutAndStop = true;
    } else {
        m_currentMusic->stop();
        m_currentMusicPath.clear();
        m_musicFading = false;
    }
}

void AudioManager::pauseMusic() {
    if (m_currentMusic && m_currentMusic->getStatus() == sf::Music::Status::Playing) {
        m_currentMusic->pause();
    }
}

void AudioManager::resumeMusic() {
    if (m_currentMusic && m_currentMusic->getStatus() == sf::Music::Status::Paused) {
        m_currentMusic->play();
    }
}

bool AudioManager::isMusicPlaying() const {
    return m_currentMusic && m_currentMusic->getStatus() == sf::Music::Status::Playing;
}

// ========== 音量控制 ==========

void AudioManager::setMasterVolume(float volume) {
    m_masterVolume = std::clamp(volume, 0.0f, 1.0f);
    applyMusicVolume();
    applySoundVolumes(); // 更新所有活跃音效音量
}

void AudioManager::setBGMVolume(float volume) {
    m_bgmVolume = std::clamp(volume, 0.0f, 1.0f);
    applyMusicVolume();
}

void AudioManager::setSFXVolume(float volume) {
    m_sfxVolume = std::clamp(volume, 0.0f, 1.0f);
    applySoundVolumes(); // 更新所有活跃音效音量
}

float AudioManager::getMasterVolume() const {
    return m_masterVolume;
}

float AudioManager::getBGMVolume() const {
    return m_bgmVolume;
}

float AudioManager::getSFXVolume() const {
    return m_sfxVolume;
}

// ========== 更新 ==========

void AudioManager::update(float deltaTime) {
    // 更新音乐淡入淡出
    if (m_musicFading && m_currentMusic) {
        m_musicFadeTimer += deltaTime;

        float progress = std::clamp(m_musicFadeTimer / m_musicFadeDuration, 0.0f, 1.0f);
        float currentVolume = m_musicFadeStartVolume + (m_musicFadeTargetVolume - m_musicFadeStartVolume) * progress;

        m_currentMusic->setVolume(currentVolume * 100.0f);

        if (progress >= 1.0f) {
            m_musicFading = false;

            if (m_musicFadeOutAndStop) {
                m_currentMusic->stop();
                m_currentMusicPath.clear();
            }
        }
    }

    // 定期清理已停止的音效
    cleanupStoppedSounds();
}

// ========== 角色特定音效 ==========

bool AudioManager::loadCharacterSounds(const std::string &characterId, const std::string & /*configPath*/) {
    // 音效已在 ResourceManager 中按 {characterId}_{soundType} 格式加载
    // 此方法保留用于兼容性，但不再需要从配置文件加载映射
    std::cout << "AudioManager: Character sounds for " << characterId
              << " are loaded in ResourceManager" << std::endl;
    return true;
}

bool AudioManager::playCharacterSound(const std::string &characterId, const std::string &soundType, float volumeScale) {
    // 直接构造 soundId: {characterId}_{soundType}
    std::string soundId = characterId + "_" + soundType;

    // 播放音效
    return playSound(soundId, volumeScale);
}

// ========== 场景背景音乐 ==========

bool AudioManager::playSceneMusic(const std::string &sceneType, float fadeInDuration) {
    // 预定义的场景音乐路径
    static const std::unordered_map<std::string, std::string> sceneMusicMap = {
        {"menu", "assets/music/menu.ogg"},
        {"battle", "assets/music/battle.ogg"},
        {"victory", "assets/music/victory_theme.ogg"},
        {"result", "assets/music/result_theme.ogg"}};

    auto it = sceneMusicMap.find(sceneType);
    if (it == sceneMusicMap.end()) {
        std::cerr << "AudioManager: Unknown scene type for music: " << sceneType << std::endl;
        return false;
    }

    return playMusic(it->second, true, fadeInDuration);
}

// ========== 私有方法 ==========

bool AudioManager::loadVolumeFromConfig(const std::string &configPath) {
    // 使用 JsonLoader 加载配置文件
    nlohmann::json config;
    std::ifstream file(configPath);

    if (!file.is_open()) {
        std::cerr << "AudioManager: Failed to open config file: " << configPath << std::endl;
        return false;
    }

    try {
        file >> config;
    } catch (const nlohmann::json::parse_error &e) {
        std::cerr << "AudioManager: Failed to parse config file: " << e.what() << std::endl;
        return false;
    }

    // 尝试读取音频配置
    if (config.contains("audio")) {
        const auto &audio = config["audio"];

        if (audio.contains("masterVolume")) {
            m_masterVolume = audio["masterVolume"].get<float>();
        }
        if (audio.contains("bgmVolume")) {
            m_bgmVolume = audio["bgmVolume"].get<float>();
        }
        if (audio.contains("sfxVolume")) {
            m_sfxVolume = audio["sfxVolume"].get<float>();
        }
        return true;
    }

    return false;
}

void AudioManager::applyMusicVolume() {
    if (m_currentMusic && !m_musicFading) {
        float volume = m_masterVolume * m_bgmVolume * 100.0f;
        m_currentMusic->setVolume(std::clamp(volume, 0.0f, 100.0f));
    }
}

void AudioManager::applySoundVolumes() {
    // 更新所有活跃音效的音量
    // 注意：音效在播放时记录了 volumeScale，但这里无法获取
    // 所以只能按当前音量设置重新计算（假设 volumeScale = 1.0）
    float finalVolume = std::clamp(m_masterVolume * m_sfxVolume * 100.0f, 0.0f, 100.0f);
    for (auto &[id, sound] : m_activeSounds) {
        if (sound && sound->getStatus() == sf::Sound::Status::Playing) {
            sound->setVolume(finalVolume);
        }
    }
}

void AudioManager::cleanupStoppedSounds() {
    m_activeSounds.erase(
        std::remove_if(m_activeSounds.begin(), m_activeSounds.end(),
                       [](const auto &pair) {
                           const auto &[id, sound] = pair;
                           return !sound || sound->getStatus() == sf::Sound::Status::Stopped;
                       }),
        m_activeSounds.end());
}

} // namespace SamuraiFight
