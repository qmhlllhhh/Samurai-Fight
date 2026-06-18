#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace SamuraiFight {

/**
 * @file ResourceManager.h
 * @brief 资源管理器
 */

/**
 * @class ResourceManager
 * @brief 加载和缓存游戏资源（纹理、字体、音频）
 *
 * 使用单例模式和缓存机制避免重复加载资源
 */
class ResourceManager {
  public:
    /**
     * @brief 获取单例实例
     *
     * @return ResourceManager& 单例实例引用
     */
    static ResourceManager &getInstance();

    /**
     * @brief 构造函数（私有，用于单例）
     */
    ResourceManager();

    /**
     * @brief 析构函数
     */
    ~ResourceManager();

    /**
     * @brief 初始化资源管理器
     *
     * 加载必要的默认资源
     *
     * @return true 初始化成功
     * @return false 初始化失败
     */
    bool initialize();

    /**
     * @brief 加载纹理
     *
     * @param id 纹理标识符
     * @param path 文件路径
     * @return true 加载成功
     * @return false 加载失败
     */
    bool loadTexture(const std::string &id, const std::string &path, bool smooth = false);

    /**
     * @brief 获取纹理
     *
     * @param id 纹理标识符
     * @return const sf::Texture& 纹理引用
     * @throws std::out_of_range 如果纹理不存在
     */
    const sf::Texture &getTexture(const std::string &id) const;

    /**
     * @brief 检查纹理是否存在
     *
     * @param id 纹理标识符
     * @return true 存在
     * @return false 不存在
     */
    bool hasTexture(const std::string &id) const;

    /**
     * @brief 加载字体
     *
     * @param id 字体标识符
     * @param path 文件路径
     * @return true 加载成功
     * @return false 加载失败
     */
    bool loadFont(const std::string &id, const std::string &path);

    /**
     * @brief 获取字体
     *
     * @param id 字体标识符
     * @return const sf::Font& 字体引用
     * @throws std::out_of_range 如果字体不存在
     */
    const sf::Font &getFont(const std::string &id) const;

    /**
     * @brief 检查字体是否存在
     *
     * @param id 字体标识符
     * @return true 存在
     * @return false 不存在
     */
    bool hasFont(const std::string &id) const;

    /**
     * @brief 获取默认字体
     *
     * @return const sf::Font& 默认字体引用
     */
    const sf::Font &getDefaultFont() const;

    /**
     * @brief 加载音效缓冲
     *
     * @param id 音效标识符
     * @param path 文件路径
     * @return true 加载成功
     * @return false 加载失败
     */
    bool loadSoundBuffer(const std::string &id, const std::string &path);

    /**
     * @brief 获取音效缓冲
     *
     * @param id 音效标识符
     * @return const sf::SoundBuffer& 音效缓冲引用
     * @throws std::out_of_range 如果音效不存在
     */
    const sf::SoundBuffer &getSoundBuffer(const std::string &id) const;

    /**
     * @brief 检查音效是否存在
     *
     * @param id 音效标识符
     * @return true 存在
     * @return false 不存在
     */
    bool hasSoundBuffer(const std::string &id) const;

    /**
     * @brief 清除所有缓存的资源
     */
    void clear();

    /**
     * @brief 清除指定纹理
     *
     * @param id 纹理标识符
     */
    void removeTexture(const std::string &id);

    /**
     * @brief 清除指定字体
     *
     * @param id 字体标识符
     */
    void removeFont(const std::string &id);

    /**
     * @brief 清除指定音效
     *
     * @param id 音效标识符
     */
    void removeSoundBuffer(const std::string &id);

  private:
    std::unordered_map<std::string, sf::Texture> m_textures;         ///< 纹理缓存
    std::unordered_map<std::string, sf::Font> m_fonts;               ///< 字体缓存
    std::unordered_map<std::string, sf::SoundBuffer> m_soundBuffers; ///< 音效缓存

    std::string m_defaultFontId; ///< 默认字体ID

    /**
     * @brief 创建默认字体（如果无法加载外部字体）
     *
     * @return true 成功创建
     * @return false 创建失败
     */
    bool createDefaultFont();

    // 禁用拷贝和移动
    ResourceManager(const ResourceManager &) = delete;
    ResourceManager &operator=(const ResourceManager &) = delete;
    ResourceManager(ResourceManager &&) = delete;
    ResourceManager &operator=(ResourceManager &&) = delete;
};

} // namespace SamuraiFight