#pragma once

#include "CharacterStateType.h"
#include "CharacterState.h"
#include <memory>
#include <unordered_map>
#include <functional>

namespace SamuraiFight {

// 前向声明
class Character;

/**
 * @file StateMachine.h
 * @brief 状态机
 */

/**
 * @class StateMachine
 * @brief 管理角色状态切换
 */
class StateMachine {
public:
    /**
     * @brief 构造函数
     *
     * @param owner 所属角色
     */
    StateMachine(Character* owner);

    /**
     * @brief 析构函数
     */
    ~StateMachine();

    /**
     * @brief 切换状态
     *
     * @param stateType 新状态类型
     */
    void changeState(CharacterStateType stateType);

    /**
     * @brief 更新当前状态
     *
     * @param deltaTime 时间增量
     */
    void update(float deltaTime);

    /**
     * @brief 获取当前状态
     *
     * @return CharacterState* 当前状态指针
     */
    CharacterState* getCurrentState() const;

    /**
     * @brief 获取当前状态类型
     *
     * @return CharacterStateType 状态类型
     */
    CharacterStateType getCurrentStateType() const;

private:
    /**
     * @brief 创建状态实例
     *
     * @param stateType 状态类型
     * @return std::unique_ptr<CharacterState> 状态实例
     */
    std::unique_ptr<CharacterState> createState(CharacterStateType stateType);

    Character* m_owner;                                       ///< 所属角色
    std::unique_ptr<CharacterState> m_currentState;           ///< 当前状态
    CharacterStateType m_currentStateType;                    ///< 当前状态类型
};

} // namespace SamuraiFight