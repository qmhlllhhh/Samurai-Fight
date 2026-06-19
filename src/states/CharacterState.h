#pragma once

#include "CharacterStateType.h"
#include <memory>

namespace SamuraiFight {

// 前向声明
class Character;

/**
 * @file CharacterState.h
 * @brief 角色状态基类
 */

/**
 * @class CharacterState
 * @brief 所有角色状态的基类
 *
 * 定义状态的基本接口，包括进入、退出、更新等
 */
class CharacterState {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     * @param type 状态类型
     */
    CharacterState(Character* owner, CharacterStateType type);

    /**
     * @brief 虚析构函数
     */
    virtual ~CharacterState();

    /**
     * @brief 进入状态时调用
     */
    virtual void onEnter();

    /**
     * @brief 退出状态时调用
     */
    virtual void onExit();

    /**
     * @brief 更新状态逻辑
     *
     * @param deltaTime 时间增量
     */
    virtual void update(float deltaTime);

    /**
     * @brief 获取状态类型
     *
     * @return CharacterStateType 状态类型
     */
    CharacterStateType getType() const;

    /**
     * @brief 获取所属角色
     *
     * @return Character* 角色指针
     */
    Character* getOwner() const;

protected:
    Character* m_owner;              ///< 所属角色
    CharacterStateType m_type;       ///< 状态类型
    int m_frameCount;                ///< 当前帧计数
};

} // namespace SamuraiFight