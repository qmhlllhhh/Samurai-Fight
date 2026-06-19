#include "CharacterFactory.h"
#include <iostream>

namespace SamuraiFight {

CharacterFactory &CharacterFactory::getInstance() {
    static CharacterFactory instance;
    return instance;
}

bool CharacterFactory::loadCharacterData(const std::string &characterId, const std::string &configPath) {
    // 检查是否已加载
    if (hasCharacterData(characterId)) {
        std::cout << "CharacterFactory: Character " << characterId << " already loaded" << std::endl;
        return true;
    }

    // 加载角色数据
    CharacterData data;
    if (!data.loadFromJson(configPath)) {
        std::cerr << "CharacterFactory: Failed to load character " << characterId << std::endl;
        return false;
    }

    // 缓存角色数据
    m_characterData[characterId] = data;
    std::cout << "CharacterFactory: Loaded character " << characterId << std::endl;
    return true;
}

std::unique_ptr<Character> CharacterFactory::createCharacter(const std::string &characterId, int playerIndex) {
    // 检查角色数据是否存在
    if (!hasCharacterData(characterId)) {
        std::cerr << "CharacterFactory: Character " << characterId << " not found" << std::endl;
        return nullptr;
    }

    // 创建角色实例
    const CharacterData &data = m_characterData[characterId];
    return std::make_unique<Character>(data, playerIndex);
}

bool CharacterFactory::hasCharacterData(const std::string &characterId) const {
    return m_characterData.find(characterId) != m_characterData.end();
}

const CharacterData *CharacterFactory::getCharacterData(const std::string &characterId) const {
    auto it = m_characterData.find(characterId);
    if (it != m_characterData.end()) {
        return &it->second;
    }
    return nullptr;
}

bool CharacterFactory::initialize() {

    bool res = 1;
    // 加载角色数据
    if (!hasCharacterData("musashi")) {
        res &= loadCharacterData("musashi", "assets/characters/musashi/config.json");
    }
    if (!hasCharacterData("sakura")) {
        res &= loadCharacterData("sakura", "assets/characters/sakura/config.json");
    }

    return res;
}

} // namespace SamuraiFight