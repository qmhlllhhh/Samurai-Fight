#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../scenes/Scene.h"

namespace SamuraiFight {

/**
 * @file SceneManager.h
 * @brief 场景管理器
 */

/**
 * @class SceneManager
 * @brief 管理场景栈和场景切换
 *
 * 支持场景栈操作：
 * - pushScene: 将新场景压入栈顶
 * - popScene: 移除栈顶场景
 * - changeScene: 替换栈顶场景
 */
class SceneManager {
public:
    /**
     * @brief 构造函数
     */
    SceneManager();

    /**
     * @brief 析构函数
     */
    ~SceneManager();

    /**
     * @brief 将场景压入栈
     *
     * 新场景成为当前场景，原当前场景暂停
     *
     * @param scene 要压入的场景
     */
    void pushScene(std::unique_ptr<Scene> scene);

    /**
     * @brief 移除栈顶场景
     *
     * 下一场景恢复执行
     */
    void popScene();

    /**
     * @brief 替换栈顶场景
     *
     * @param scene 新场景
     */
    void changeScene(std::unique_ptr<Scene> scene);

    /**
     * @brief 清空场景栈
     */
    void clearScenes();

    /**
     * @brief 获取当前场景
     *
     * @return Scene* 当前场景指针（如果栈为空则返回nullptr）
     */
    Scene* getCurrentScene() const;

    /**
     * @brief 检查场景栈是否为空
     *
     * @return true 栈为空
     * @return false 栈不为空
     */
    bool isEmpty() const;

    /**
     * @brief 获取场景栈大小
     *
     * @return size_t 场景数量
     */
    size_t getSceneCount() const;

    /**
     * @brief 处理当前场景的事件
     *
     * @param window 游戏窗口
     */
    void handleEvents(sf::RenderWindow& window);

    /**
     * @brief 更新当前场景的逻辑
     *
     * @param deltaTime 固定时间步长
     */
    void update(float deltaTime);

    /**
     * @brief 渲染所有场景（从底部到顶部）
     *
     * @param window 游戏窗口
     */
    void render(sf::RenderWindow& window);

private:
    std::vector<std::unique_ptr<Scene>> m_sceneStack;  ///< 场景栈
};

} // namespace SamuraiFight