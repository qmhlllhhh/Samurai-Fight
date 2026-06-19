#pragma once

#include "../entities/CharacterData.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace SamuraiFight {

/**
 * @file HitboxComponent.h
 * @brief 碰撞框组件
 */

/**
 * @struct Hitbox
 * @brief 碰撞框结构
 */
struct Hitbox {
    sf::FloatRect rect;            ///< 碰撞矩形
    std::string type;              ///< 类型 (hurtbox/hitbox)
    int currentFrame;              ///< 当前帧（用于激活检查）
    std::vector<int> activeFrames; ///< 激活帧列表

    /**
     * @brief 检查当前帧是否激活
     *
     * @return true 激活
     * @return false 未激活
     */
    bool isActive() const;
};

/**
 * @class HitboxComponent
 * @brief 管理角色的碰撞框
 *
 * 包含受击框（hurtbox）和攻击框（hitbox）
 */
class HitboxComponent {
  public:
    /**
     * @brief 构造函数
     */
    HitboxComponent();

    /**
     * @brief 析构函数
     */
    ~HitboxComponent();

    /**
     * @brief 加载碰撞框数据
     *
     * @param state 状态名称
     * @param hitboxData 碰撞框数据列表
     */
    void loadHitboxes(const std::string &state, const std::vector<HitboxData> &hitboxData);

    /**
     * @brief 更新碰撞框位置
     *
     * @param position 角色位置
     * @param state 当前状态
     * @param frame 当前动画帧
     * @param facingRight 是否朝向右
     */
    void update(const sf::Vector2f &position, const std::string &state, int frame, bool facingRight);

    /**
     * @brief 获取所有受击框
     *
     * @return std::vector<Hitbox> 受击框列表
     */
    std::vector<Hitbox> getHurtboxes() const;

    /**
     * @brief 获取所有攻击框
     *
     * @return std::vector<Hitbox> 攻击框列表
     */
    std::vector<Hitbox> getHitboxes() const;

    /**
     * @brief 检查是否与另一个碰撞框相交
     *
     * @param other 另一个碰撞框
     * @return true 相交
     * @return false 不相交
     */
    bool intersects(const Hitbox &other) const;

    /**
     * @brief 调试渲染碰撞框
     *
     * @param window 游戏窗口
     */
    void renderDebug(sf::RenderWindow &window);

  private:
    std::unordered_map<std::string, std::vector<Hitbox>> m_hitboxesByState;
    std::vector<Hitbox> m_currentHitboxes;
    sf::Vector2f m_position;
    bool m_facingRight;
};

} // namespace SamuraiFight