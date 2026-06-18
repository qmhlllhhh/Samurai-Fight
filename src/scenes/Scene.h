#pragma once

#include <SFML/Graphics.hpp>

namespace SamuraiFight {

/**
 * @file Scene.h
 * @brief 场景基类
 */

/**
 * @class Scene
 * @brief 所有游戏场景的基类
 *
 * 定义场景的基本接口，包括进入、退出、事件处理、更新和渲染
 */
class Scene {
  public:
    /**
     * @brief 构造函数
     */
    Scene();

    /**
     * @brief 虚析构函数
     */
    virtual ~Scene();

    /**
     * @brief 进入场景时调用
     *
     * 用于初始化场景状态、加载资源等
     */
    virtual void onEnter();

    /**
     * @brief 退出场景时调用
     *
     * 用于清理场景状态、释放资源等
     */
    virtual void onExit();

    /**
     * @brief 暂停场景时调用（当新场景叠加在当前场景之上）
     */
    virtual void onPause();

    /**
     * @brief 恢复场景时调用（当上层场景被移除）
     */
    virtual void onResume();

    /**
     * @brief 处理事件
     *
     * @param window 游戏窗口
     */
    virtual void handleEvents(sf::RenderWindow &window) = 0;

    /**
     * @brief 更新场景逻辑
     *
     * @param deltaTime 固定时间步长
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief 渲染场景
     *
     * @param window 游戏窗口
     */
    virtual void render(sf::RenderWindow &window) = 0;

    /**
     * @brief 获取场景是否完成
     *
     * 用于判断是否需要切换到下一个场景
     *
     * @return true 场景已完成
     * @return false 场景未完成
     */
    virtual int getPopSceneCount() const;

    /**
     * @brief 获取下一个场景
     *
     * @return std::unique_ptr<Scene> 下一个场景
     */
    virtual std::unique_ptr<Scene> getNextScene();

  protected:
    int m_popSceneCount;
    std::unique_ptr<Scene> m_nextScene; ///< 下一个场景指针
};

} // namespace SamuraiFight