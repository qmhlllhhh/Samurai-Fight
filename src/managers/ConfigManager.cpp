#include "ConfigManager.h"
#include "../core/Constants.h"

#include <iostream>

namespace SamuraiFight {

ConfigManager::ConfigManager() {
    // 初始化默认配置
    m_gameConfig = nlohmann::json::object();
    m_keyBindings = nlohmann::json::object();
}

ConfigManager::~ConfigManager() {
}

bool ConfigManager::initialize() {
    // 加载默认配置文件
    bool gameConfigLoaded = loadGameConfig("assets/config/game_config.json");
    bool keyBindingsLoaded = loadKeyBindings("assets/config/keybindings.json");

    // 如果配置加载失败，使用默认值
    if (!gameConfigLoaded) {
        std::cerr << "ConfigManager: Using default game config" << std::endl;
        m_gameConfig = {
            {"window", {
                {"width", WINDOW_WIDTH},
                {"height", WINDOW_HEIGHT},
                {"title", WINDOW_TITLE},
                {"fullscreen", false},
                {"vsync", true}
            }},
            {"gameplay", {
                {"targetFPS", TARGET_FPS},
                {"roundTime", ROUND_TIME},
                {"winsNeeded", WINS_NEEDED}
            }},
            {"audio", {
                {"masterVolume", DEFAULT_MASTER_VOLUME},
                {"bgmVolume", DEFAULT_BGM_VOLUME},
                {"sfxVolume", DEFAULT_SFX_VOLUME}
            }}
        };
    }

    if (!keyBindingsLoaded) {
        std::cerr << "ConfigManager: Using default key bindings" << std::endl;
        // 键位配置将稍后由 InputManager 处理
    }

    return gameConfigLoaded || keyBindingsLoaded;
}

bool ConfigManager::loadGameConfig(const std::string& path) {
    if (!JsonLoader::loadFromFile(path, m_gameConfig)) {
        std::cerr << "ConfigManager: Failed to load game config from " << path << std::endl;
        return false;
    }

    std::cout << "ConfigManager: Game config loaded from " << path << std::endl;
    return true;
}

bool ConfigManager::loadKeyBindings(const std::string& path) {
    if (!JsonLoader::loadFromFile(path, m_keyBindings)) {
        std::cerr << "ConfigManager: Failed to load key bindings from " << path << std::endl;
        return false;
    }

    std::cout << "ConfigManager: Key bindings loaded from " << path << std::endl;
    return true;
}

const nlohmann::json& ConfigManager::getGameConfig() const {
    return m_gameConfig;
}

const nlohmann::json& ConfigManager::getKeyBindings() const {
    return m_keyBindings;
}

int ConfigManager::getWindowWidth() const {
    if (m_gameConfig.contains("window") && m_gameConfig["window"].contains("width")) {
        return m_gameConfig["window"]["width"].get<int>();
    }
    return WINDOW_WIDTH;
}

int ConfigManager::getWindowHeight() const {
    if (m_gameConfig.contains("window") && m_gameConfig["window"].contains("height")) {
        return m_gameConfig["window"]["height"].get<int>();
    }
    return WINDOW_HEIGHT;
}

std::string ConfigManager::getWindowTitle() const {
    if (m_gameConfig.contains("window") && m_gameConfig["window"].contains("title")) {
        return m_gameConfig["window"]["title"].get<std::string>();
    }
    return WINDOW_TITLE;
}

bool ConfigManager::isFullscreen() const {
    if (m_gameConfig.contains("window") && m_gameConfig["window"].contains("fullscreen")) {
        return m_gameConfig["window"]["fullscreen"].get<bool>();
    }
    return false;
}

bool ConfigManager::isVSyncEnabled() const {
    if (m_gameConfig.contains("window") && m_gameConfig["window"].contains("vsync")) {
        return m_gameConfig["window"]["vsync"].get<bool>();
    }
    return true;
}

int ConfigManager::getTargetFPS() const {
    if (m_gameConfig.contains("gameplay") && m_gameConfig["gameplay"].contains("targetFPS")) {
        return m_gameConfig["gameplay"]["targetFPS"].get<int>();
    }
    return TARGET_FPS;
}

int ConfigManager::getRoundTime() const {
    if (m_gameConfig.contains("gameplay") && m_gameConfig["gameplay"].contains("roundTime")) {
        return m_gameConfig["gameplay"]["roundTime"].get<int>();
    }
    return ROUND_TIME;
}

int ConfigManager::getWinsNeeded() const {
    if (m_gameConfig.contains("gameplay") && m_gameConfig["gameplay"].contains("winsNeeded")) {
        return m_gameConfig["gameplay"]["winsNeeded"].get<int>();
    }
    return WINS_NEEDED;
}

float ConfigManager::getMasterVolume() const {
    if (m_gameConfig.contains("audio") && m_gameConfig["audio"].contains("masterVolume")) {
        return m_gameConfig["audio"]["masterVolume"].get<float>();
    }
    return DEFAULT_MASTER_VOLUME;
}

float ConfigManager::getBGMVolume() const {
    if (m_gameConfig.contains("audio") && m_gameConfig["audio"].contains("bgmVolume")) {
        return m_gameConfig["audio"]["bgmVolume"].get<float>();
    }
    return DEFAULT_BGM_VOLUME;
}

float ConfigManager::getSFXVolume() const {
    if (m_gameConfig.contains("audio") && m_gameConfig["audio"].contains("sfxVolume")) {
        return m_gameConfig["audio"]["sfxVolume"].get<float>();
    }
    return DEFAULT_SFX_VOLUME;
}

} // namespace SamuraiFight