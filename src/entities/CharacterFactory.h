#pragma once

#include "Character.h"
#include "CharacterData.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace SamuraiFight {

/**
 * @file CharacterFactory.h
 * @brief 角色工厂
 */

/**
 * @class CharacterFactory
 * @brief 创建角色实例的工厂类
 *
 * 负责加载角色数据并创建角色实例
 */
class CharacterFactory {
  public:
    /**
     * @brief 获取单例实例
     *
     * @return CharacterFactory& 单例实例
     */
    static CharacterFactory &getInstance();

    /**
     * @brief 加载角色数据
     *
     * @param characterId 角色ID
     * @param configPath 配置文件路径
     * @return true 加载成功
     * @return false 加载失败
     */
    bool loadCharacterData(const std::string &characterId, const std::string &configPath);

    /**
     * @brief 创建角色实例
     *
     * @param characterId 角色ID
     * @param playerIndex 玩家索引
     * @return std::unique_ptr<Character> 角色实例
     */
    std::unique_ptr<Character> createCharacter(const std::string &characterId, int playerIndex);

    /**
     * @brief 检查角色数据是否存在
     *
     * @param characterId 角色ID
     * @return true 存在
     * @return false 不存在
     */
    bool hasCharacterData(const std::string &characterId) const;

    /**
     * @brief 获取角色数据
     *
     * @param characterId 角色ID
     * @return const CharacterData* 角色数据指针
     */
    const CharacterData *getCharacterData(const std::string &characterId) const;

    /**
     *
     *
     */

    bool initialize();

  private:
    /**
     * @brief 私有构造函数
     */
    CharacterFactory() = default;

    /**
     * @brief 私有析构函数
     */
    ~CharacterFactory() = default;

    // 禁用拷贝和移动
    CharacterFactory(const CharacterFactory &) = delete;
    CharacterFactory &operator=(const CharacterFactory &) = delete;
    CharacterFactory(CharacterFactory &&) = delete;
    CharacterFactory &operator=(CharacterFactory &&) = delete;

    std::unordered_map<std::string, CharacterData> m_characterData; ///< 角色数据缓存
};

} // namespace SamuraiFight