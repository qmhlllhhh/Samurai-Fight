#pragma once

#include "../core/Constants.h"
#include "../managers/SceneManager.h"
#include "../utils/utils.h"
#include "../entities/Character.h"
#include "../input/InputManager.h"
#include "../ui/HealthBar.h"
#include "../ui/StaminaBar.h"
#include "../battle/MatchManager.h"
#include "../battle/MatchHud.h"
#include "../effects/ScreenShake.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

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
     * @brief 默认构造函数
     */
    BattleScene();

    /**
     * @brief 带角色选择的构造函数
     *
     * @param player1CharacterId 玩家1角色ID
     * @param player2CharacterId 玩家2角色ID
     */
    BattleScene(const std::string& player1CharacterId, const std::string& player2CharacterId);

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
    /**
     * @brief 初始化角色
     */
    void initializeCharacters();

    /**
     * @brief 初始化背景
     */
    void initializeBackground();

    /**
     * @brief 检查角色间的碰撞
     */
    void checkCollisions();

    /**
     * @brief 检查两个角色的攻击碰撞
     *
     * @param attacker 攻击者索引
     * @param defender 防御者索引
     */
    void checkAttackCollision(int attacker, int defender);

    std::string m_backgroundId[BACKGROUND_COUNT];
    std::unique_ptr<sf::Sprite> m_background; ///< 战斗背景
    sf::RenderWindow *m_window;               ///< 游戏窗口指针

    // 角色选择
    std::string m_player1CharacterId;  ///< 玩家1角色ID
    std::string m_player2CharacterId;  ///< 玩家2角色ID

    // 角色系统
    std::unique_ptr<Character> m_characters[2];  ///< 两个角色
    std::unique_ptr<InputManager> m_inputManager; ///< 输入管理器

    // UI
    std::unique_ptr<HealthBar> m_healthBars[2];  ///< 两个玩家的血条
    std::unique_ptr<StaminaBar> m_staminaBars[2]; ///< 两个玩家的体力条

    // 调试信息
    std::unique_ptr<sf::Text> m_debugText; ///< 调试文本
    bool m_showDebug;     ///< 是否显示调试信息

    // 比赛系统（阶段4）
    std::unique_ptr<MatchManager> m_matchManager; ///< 比赛管理器
    std::unique_ptr<MatchHud> m_matchHud;         ///< 比赛HUD

    /**
     * @brief 比赛结束时处理（切结果场景）
     */
    void onMatchEnd();

    // 屏幕震动效果
    ScreenShake m_screenShake; ///< 屏幕震动
};
} // namespace SamuraiFight