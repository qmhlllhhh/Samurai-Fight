#pragma once

#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

namespace SamuraiFight {

/**
 * @file ResultScene.h
 * @brief 结果场景
 */

/**
 * @class ResultScene
 * @brief 比赛结果界面
 *
 * 显示胜者与比分，提供重赛 / 返回角色选择 / 返回主菜单。
 * 参照 MainMenuScene 的键盘+鼠标交互。
 */
class ResultScene : public Scene {
public:
    /**
     * @brief 构造函数
     *
     * @param winner 胜者索引（0/1，-1=平局）
     * @param roundsWonP1 玩家1胜场
     * @param roundsWonP2 玩家2胜场
     * @param p1CharId 玩家1角色ID（重赛用）
     * @param p2CharId 玩家2角色ID（重赛用）
     */
    ResultScene(int winner, int roundsWonP1, int roundsWonP2,
                const std::string& p1CharId, const std::string& p2CharId);
    ~ResultScene() override;

    void onEnter() override;
    void onExit() override;
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void initMenuItems();
    void updateMenuSelection(const sf::Vector2f& mousePos);
    void selectPreviousItem();
    void selectNextItem();
    void executeCurrentItem();

    std::unique_ptr<sf::Text> m_titleText;                    ///< 胜者标题
    std::unique_ptr<sf::Text> m_scoreText;                    ///< 比分
    std::vector<std::unique_ptr<sf::Text>> m_menuItems;       ///< 菜单项（Rematch/CharSelect/MainMenu）

    int m_selectedIndex;
    sf::Color m_normalColor;
    sf::Color m_selectedColor;

    float m_titleY;
    float m_scoreY;
    float m_menuStartY;
    float m_menuSpacing;

    bool m_mouseHovering;

    int m_winner;
    int m_roundsWon[2];
    std::string m_p1CharId;
    std::string m_p2CharId;
};

} // namespace SamuraiFight
