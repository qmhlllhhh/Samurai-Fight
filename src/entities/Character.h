#pragma once

#include "Entity.h"
#include "CharacterData.h"
#include "../states/CharacterStateType.h"
#include "../components/AnimationComponent.h"
#include "../components/HitboxComponent.h"
#include "../input/InputBuffer.h"
#include <memory>

namespace SamuraiFight {

// 前向声明
class CharacterState;
class StateMachine;

/**
 * @file Character.h
 * @brief 角色类
 */

/**
 * @class Character
 * @brief 游戏角色类
 *
 * 包含状态机、动画、碰撞框等组件
 */
class Character : public Entity {
public:
    /**
     * @brief 构造函数
     *
     * @param data 角色数据
     * @param playerIndex 玩家索引 (0或1)
     */
    Character(const CharacterData& data, int playerIndex);

    /**
     * @brief 析构函数
     */
    ~Character() override;

    /**
     * @brief 处理输入
     *
     * @param moveLeft 是否向左移动
     * @param moveRight 是否向右移动
     * @param jump 是否跳跃
     * @param crouch 是否下蹲
     * @param attackLight 轻攻击
     * @param attackMedium 中攻击
     * @param attackHeavy 重攻击
     */
    void handleInput(bool moveLeft, bool moveRight, bool jump, bool crouch,
                     bool attackLight, bool attackMedium, bool attackHeavy);

    /**
     * @brief 更新角色逻辑
     *
     * @param deltaTime 时间增量
     */
    void update(float deltaTime) override;

    /**
     * @brief 渲染角色
     *
     * @param window 游戏窗口
     */
    void render(sf::RenderWindow& window) override;

    /**
     * @brief 切换状态
     *
     * @param stateType 新状态类型
     */
    void changeState(CharacterStateType stateType);

    /**
     * @brief 播放动画
     *
     * @param name 动画名称
     */
    void playAnimation(const std::string& name);

    /**
     * @brief 获取移动速度
     *
     * @return float 移动速度
     */
    float getMoveSpeed() const;

    /**
     * @brief 获取跳跃高度
     *
     * @return float 跳跃高度
     */
    float getJumpHeight() const;

    /**
     * @brief 获取当前状态类型
     *
     * @return CharacterStateType 状态类型
     */
    CharacterStateType getCurrentStateType() const;

    /**
     * @brief 获取当前动画帧
     *
     * @return int 帧索引
     */
    int getCurrentAnimationFrame() const;

    /**
     * @brief 检查是否朝向右边
     *
     * @return true 朝右
     * @return false 朝左
     */
    bool isFacingRight() const;

    /**
     * @brief 设置朝向
     *
     * @param facingRight 是否朝右
     */
    void setFacingRight(bool facingRight);

    /**
     * @brief 获取玩家索引
     *
     * @return int 玩家索引
     */
    int getPlayerIndex() const;

    /**
     * @brief 获取角色数据
     *
     * @return const CharacterData& 角色数据
     */
    const CharacterData& getData() const;

    /**
     * @brief 获取输入缓冲
     *
     * @return InputBuffer& 输入缓冲引用
     */
    InputBuffer& getInputBuffer();

    /**
     * @brief 获取当前帧号
     *
     * @return int 帧号
     */
    int getFrameCount() const;

    /**
     * @brief 获取碰撞框组件
     *
     * @return HitboxComponent* 碰撞框组件指针
     */
    HitboxComponent* getHitboxComponent();

    /**
     * @brief 获取碰撞框组件（常量版本）
     *
     * @return const HitboxComponent* 碰撞框组件指针
     */
    const HitboxComponent* getHitboxComponent() const;

    /**
     * @brief 获取状态机
     *
     * @return StateMachine* 状态机指针
     */
    StateMachine* getStateMachine();

    /**
     * @brief 检查当前是否可以取消攻击
     *
     * @return true 可以取消
     * @return false 不能取消
     */
    bool canCancelAttack() const;

    /**
     * @brief 获取当前攻击是否处于判定帧
     *
     * @return true 在判定帧
     * @return false 不在判定帧
     */
    bool isInAttackActiveFrames() const;

    /**
     * @brief 受到伤害
     *
     * @param damage 伤害值
     * @param stunFrames 硬直帧数
     */
    void takeDamage(float damage, int stunFrames);

    /**
     * @brief 设置是否显示调试碰撞框
     *
     * @param show 是否显示
     */
    void setShowDebugHitboxes(bool show);

    /**
     * @brief 获取是否显示调试碰撞框
     *
     * @return true 显示
     * @return false 不显示
     */
    bool isShowDebugHitboxes() const;

private:
    /**
     * @brief 初始化组件
     */
    void initializeComponents();

    /**
     * @brief 应用重力
     *
     * @param deltaTime 时间增量
     */
    void applyGravity(float deltaTime);

    /**
     * @brief 检测地面碰撞
     */
    void checkGroundCollision();

    /**
     * @brief 更新输入缓冲
     */
    void updateInputBuffer();

    CharacterData m_data;                                    ///< 角色数据
    std::unique_ptr<StateMachine> m_stateMachine;            ///< 状态机
    std::unique_ptr<AnimationComponent> m_animation;         ///< 动画组件
    std::unique_ptr<HitboxComponent> m_hitbox;               ///< 碰撞框组件
    std::unique_ptr<InputBuffer> m_inputBuffer;              ///< 输入缓冲

    std::unique_ptr<sf::Sprite> m_sprite;                    ///< 精灵
    int m_playerIndex;                                        ///< 玩家索引
    bool m_facingRight;                                       ///< 是否朝右
    int m_frameCount;                                         ///< 帧计数器

    // 输入状态
    bool m_moveLeft;
    bool m_moveRight;
    bool m_jump;
    bool m_crouch;
    bool m_attackLight;
    bool m_attackMedium;
    bool m_attackHeavy;

    // 调试选项
    bool m_showDebugHitboxes;  ///< 是否显示调试碰撞框
};

} // namespace SamuraiFight