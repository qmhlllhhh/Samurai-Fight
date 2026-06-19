#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include "../entities/CharacterData.h"

namespace SamuraiFight {

/**
 * @file AnimationComponent.h
 * @brief 动画组件
 */

/**
 * @class AnimationComponent
 * @brief 管理角色动画播放
 *
 * 支持精灵表动画，包括帧动画播放、循环控制等
 */
class AnimationComponent {
public:
    /**
     * @brief 构造函数
     *
     * @param sprite 关联的精灵
     */
    AnimationComponent(sf::Sprite& sprite);

    /**
     * @brief 析构函数
     */
    ~AnimationComponent();

    /**
     * @brief 加载动画
     *
     * @param name 动画名称
     * @param data 动画数据
     * @return true 加载成功
     * @return false 加载失败
     */
    bool loadAnimation(const std::string& name, const AnimationData& data);

    /**
     * @brief 播放指定动画
     *
     * @param name 动画名称
     */
    void play(const std::string& name);

    /**
     * @brief 更新动画
     *
     * @param deltaTime 时间增量
     */
    void update(float deltaTime);

    /**
     * @brief 检查当前动画是否完成
     *
     * @return true 动画完成
     * @return false 动画未完成
     */
    bool isFinished() const;

    /**
     * @brief 获取当前动画名称
     *
     * @return std::string 动画名称
     */
    std::string getCurrentAnimation() const;

    /**
     * @brief 获取当前帧索引
     *
     * @return int 帧索引
     */
    int getCurrentFrame() const;

    /**
     * @brief 重置动画到第一帧
     */
    void reset();

private:
    /**
     * @brief 更新精灵纹理区域
     */
    void updateSpriteRect();

    sf::Sprite& m_sprite;                                       ///< 关联的精灵
    std::unordered_map<std::string, AnimationData> m_animations; ///< 动画数据

    std::string m_currentAnimation; ///< 当前动画名称
    int m_currentFrame;             ///< 当前帧索引
    float m_frameTimer;             ///< 帧计时器
    bool m_playing;                 ///< 是否正在播放
};

} // namespace SamuraiFight