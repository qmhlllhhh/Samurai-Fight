#include "ResourceManager.h"

#include <iostream>
#include <stdexcept>

namespace SamuraiFight {

ResourceManager &ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

ResourceManager::ResourceManager()
    : m_defaultFontId("default"), m_defaultBoldFontId("defaultBold"), m_chFontId("ch") {
}

ResourceManager::~ResourceManager() {
    clear();
}

bool ResourceManager::initialize() {
    // 尝试加载默认字体

    std::string fontPath = "assets/fonts/Pixel.ttf";

    if (!loadFont(m_defaultFontId, fontPath)) {
        std::cerr << "ResourceManager: Failed to load default font from " << fontPath << std::endl;

        // 尝试创建系统默认字体
        if (!createDefaultFont()) {
            std::cerr << "ResourceManager: Warning: No default font available" << std::endl;
        }
    }
    if (!loadFont(m_defaultBoldFontId, (fontPath = "assets/fonts/Pixel-Bold.ttf"))) {
        std::cerr << "ResourceManager: Failed to load default-Bold font from " << fontPath << std::endl;
    }
    if (!loadFont(m_chFontId, (fontPath = "assets/fonts/ch.ttf"))) {
        std::cerr << "ResourceManager: Failed to load ch font from " << fontPath << std::endl;
    }

    // 尝试加载背景图
    std::string grass1Path = "assets/backgrounds/grass1.png";
    std::string grass3Path = "assets/backgrounds/grass3.png";
    std::string housePath = "assets/backgrounds/house.png";
    if (!loadTexture("grass1", grass1Path, 1) ||
        !loadTexture("grass3", grass3Path, 1) ||
        !loadTexture("house", housePath, 1)) {
        std::cerr << "ResourceManager: Warning: Some background textures failed to load" << std::endl;
    }

    // 加载音效
    std::vector<std::pair<std::string, std::string>> soundsToLoad = {
        // Combat sounds
        {"hit", "assets/sounds/combat/hit.wav"},
        {"block_success", "assets/sounds/combat/block_success.wav"},

        // Movement sounds
        {"jump", "assets/sounds/movement/jump.wav"},
        {"land", "assets/sounds/movement/land.wav"},

        // UI sounds
        {"select", "assets/sounds/ui/select.wav"},
        {"cursor", "assets/sounds/ui/cursor.wav"},
        {"cancel", "assets/sounds/ui/cancel.wav"},
        {"popup_open", "assets/sounds/ui/popup_open.wav"},
        {"popup_close", "assets/sounds/ui/popup_close.wav"},

        // Round sounds
        {"round_start", "assets/sounds/round/round_start.wav"},
        {"round_end", "assets/sounds/round/round_end.wav"},
        {"victory", "assets/sounds/round/victory.wav"},

        // Character-specific sounds - Musashi
        {"musashi_hurt", "assets/sounds/characters/musashi/hurt.wav"},
        {"musashi_death", "assets/sounds/characters/musashi/death.wav"},
        {"musashi_attack_light", "assets/sounds/characters/musashi/attack_light.wav"},
        {"musashi_attack_medium", "assets/sounds/characters/musashi/attack_medium.wav"},
        {"musashi_attack_heavy", "assets/sounds/characters/musashi/attack_heavy.wav"},
        {"musashi_block", "assets/sounds/characters/musashi/block.wav"},
        {"musashi_jump", "assets/sounds/characters/musashi/jump.wav"},
        {"musashi_land", "assets/sounds/characters/musashi/land.wav"},
        {"musashi_roll", "assets/sounds/characters/musashi/roll.wav"},

        // Character-specific sounds - Sakura
        {"sakura_hurt", "assets/sounds/characters/sakura/hurt.wav"},
        {"sakura_death", "assets/sounds/characters/sakura/death.wav"},
        {"sakura_attack_light", "assets/sounds/characters/sakura/attack_light.wav"},
        {"sakura_attack_medium", "assets/sounds/characters/sakura/attack_medium.wav"},
        {"sakura_attack_heavy", "assets/sounds/characters/sakura/attack_heavy.wav"},
        {"sakura_block", "assets/sounds/characters/sakura/block.wav"},
        {"sakura_jump", "assets/sounds/characters/sakura/jump.wav"},
        {"sakura_roll", "assets/sounds/characters/sakura/roll.wav"},
        {"sakura_land", "assets/sounds/characters/sakura/land.wav"}};

    for (const auto &[id, path] : soundsToLoad) {
        if (!loadSoundBuffer(id, path)) {
            std::cerr << "ResourceManager: Warning: Failed to load sound '" << id << "'" << std::endl;
        }
    }

    std::cout << "ResourceManager: Initialized successfully" << std::endl;
    return true;
}

bool ResourceManager::loadTexture(const std::string &id, const std::string &path, bool smooth) {
    // 检查是否已经加载
    if (hasTexture(id)) {
        std::cout << "ResourceManager: Texture '" << id << "' already loaded" << std::endl;
        return true;
    }

    // 创建纹理并加载
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        std::cerr << "ResourceManager: Failed to load texture '" << id << "' from " << path << std::endl;
        return false;
    }

    texture.setSmooth(smooth);

    // 存储纹理
    m_textures[id] = std::move(texture);
    std::cout << "ResourceManager: Loaded texture '" << id << "' from " << path << std::endl;
    return true;
}

const sf::Texture &ResourceManager::getTexture(const std::string &id) const {
    auto it = m_textures.find(id);
    if (it == m_textures.end()) {
        throw std::out_of_range("ResourceManager: Texture '" + id + "' not found");
    }
    return it->second;
}

bool ResourceManager::hasTexture(const std::string &id) const {
    return m_textures.find(id) != m_textures.end();
}

bool ResourceManager::loadFont(const std::string &id, const std::string &path) {
    // 检查是否已经加载
    if (hasFont(id)) {
        std::cout << "ResourceManager: Font '" << id << "' already loaded" << std::endl;
        return true;
    }

    // 创建字体并加载
    sf::Font font;
    if (!font.openFromFile(path)) {
        std::cerr << "ResourceManager: Failed to load font '" << id << "' from " << path << std::endl;
        return false;
    }

    // 存储字体
    m_fonts[id] = std::move(font);
    std::cout << "ResourceManager: Loaded font '" << id << "' from " << path << std::endl;
    return true;
}

const sf::Font &ResourceManager::getFont(const std::string &id) const {
    auto it = m_fonts.find(id);
    if (it == m_fonts.end()) {
        throw std::out_of_range("ResourceManager: Font '" + id + "' not found");
    }
    return it->second;
}

bool ResourceManager::hasFont(const std::string &id) const {
    return m_fonts.find(id) != m_fonts.end();
}

const sf::Font &ResourceManager::getDefaultFont() const {
    if (hasFont(m_defaultFontId)) {
        return getFont(m_defaultFontId);
    }

    // 如果默认字体不存在，尝试返回任意可用字体
    if (!m_fonts.empty()) {
        return m_fonts.begin()->second;
    }

    throw std::runtime_error("ResourceManager: No fonts available");
}

bool ResourceManager::loadSoundBuffer(const std::string &id, const std::string &path) {
    // 检查是否已经加载
    if (hasSoundBuffer(id)) {
        std::cout << "ResourceManager: Sound buffer '" << id << "' already loaded" << std::endl;
        return true;
    }

    // 创建音效缓冲并加载
    sf::SoundBuffer soundBuffer;
    if (!soundBuffer.loadFromFile(path)) {
        std::cerr << "ResourceManager: Failed to load sound buffer '" << id << "' from " << path << std::endl;
        return false;
    }

    // 存储音效缓冲
    m_soundBuffers[id] = std::move(soundBuffer);
    std::cout << "ResourceManager: Loaded sound buffer '" << id << "' from " << path << std::endl;
    return true;
}

const sf::SoundBuffer &ResourceManager::getSoundBuffer(const std::string &id) const {
    auto it = m_soundBuffers.find(id);
    if (it == m_soundBuffers.end()) {
        throw std::out_of_range("ResourceManager: Sound buffer '" + id + "' not found");
    }
    return it->second;
}

bool ResourceManager::hasSoundBuffer(const std::string &id) const {
    return m_soundBuffers.find(id) != m_soundBuffers.end();
}

void ResourceManager::clear() {
    m_textures.clear();
    m_fonts.clear();
    m_soundBuffers.clear();
    std::cout << "ResourceManager: All resources cleared" << std::endl;
}

void ResourceManager::removeTexture(const std::string &id) {
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        m_textures.erase(it);
        std::cout << "ResourceManager: Removed texture '" << id << "'" << std::endl;
    }
}

void ResourceManager::removeFont(const std::string &id) {
    auto it = m_fonts.find(id);
    if (it != m_fonts.end()) {
        m_fonts.erase(it);
        std::cout << "ResourceManager: Removed font '" << id << "'" << std::endl;
    }
}

void ResourceManager::removeSoundBuffer(const std::string &id) {
    auto it = m_soundBuffers.find(id);
    if (it != m_soundBuffers.end()) {
        m_soundBuffers.erase(it);
        std::cout << "ResourceManager: Removed sound buffer '" << id << "'" << std::endl;
    }
}

bool ResourceManager::createDefaultFont() {
    // SFML 3.0 不支持从内存创建空字体
    // 我们需要用户提供字体文件，或者使用系统字体

    // 尝试一些常见的系统字体路径
    std::vector<std::string> systemFontPaths = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/System/Library/Fonts/Helvetica.ttc"};

    for (const auto &path : systemFontPaths) {
        sf::Font font;
        if (font.openFromFile(path)) {
            m_fonts[m_defaultFontId] = std::move(font);
            std::cout << "ResourceManager: Created default font from system font: " << path << std::endl;
            return true;
        }
    }

    return false;
}

} // namespace SamuraiFight