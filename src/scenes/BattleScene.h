#pragma once

#include "../core/Constants.h"
#include "../managers/SceneManager.h"
#include "../utils/utils.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

namespace SamuraiFight {

/**
 * @file BattleScene.h
 * @brief 战斗场景
 */

/**
 * @class BattleScene
 * @brief 战斗场景
 *
 * 显示战斗界面，包括：
 * - 玩家和敌人的角色
 * - 生命值和状态显示
 * - 战斗动作和动画
 * - 键盘/鼠标输入处理
 */
class BattleScene : public Scene {
  public:
    /**
     * @brief 构造函数
     */
    BattleScene();

    /**
     * @brief 析构函数
     */
    ~BattleScene() override;

    /**
     * @brief 进入场景时初始化
     */
    void onEnter() override;

    /**
     * @brief 退出场景时清理
     */
    void onExit() override;

    /**
     * @brief 处理输入事件
     *
     * @param window 游戏窗口
     */
    void handleEvents(sf::RenderWindow &window) override;

    /**
     * @brief 更新战斗逻辑
     *
     * @param deltaTime 时间增量
     * */
    void update(float deltaTime) override;
    /**
     * @brief 渲染战斗界面
     *
     * @param window 游戏窗口
     */
    void render(sf::RenderWindow &window) override;

  private:
    std::string m_backgroundId[BACKGROUND_COUNT];
    std::unique_ptr<sf::Sprite> m_background; ///< 战斗背景
    sf::RenderWindow *m_window;               ///< 游戏窗口指针
};
} // namespace SamuraiFight