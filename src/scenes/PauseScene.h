#pragma once

#include "MainMenuScene.h"
#include "MenuItem.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

namespace SamuraiFight {

/**
 * @file PauseScene.h
 * @brief 暂停场景
 */
class PauseScene : public Scene {
  public:
    /**
     * @brief 构造函数
     */
    PauseScene();

    /**
     * @brief 析构函数
     */
    ~PauseScene() override;

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
     * @brief 更新暂停逻辑
     *
     * @param deltaTime 时间增量
     */
    void update(float deltaTime) override;

    /**
     * @brief 渲染暂停界面
     *
     * @param window 游戏窗口
     */
    void render(sf::RenderWindow &window) override;

  private:
    void initMenuItems();

    void updateMenuSelection(const sf::Vector2f &mousePos);

    void handleMenuClick();

    void selectPreviousItem();

    void selectNextItem();

    void executeCurrentItem();

    std::unique_ptr<sf::Text> m_pauseText;              ///< 暂停文本
    std::vector<std::unique_ptr<sf::Text>> m_menuItems; ///< 菜单项文本
    std::vector<MenuItem> m_menuActions;                ///< 菜单项动作

    int m_selectedIndex;       ///< 当前选中项索引
    sf::Color m_normalColor;   ///< 正常颜色
    sf::Color m_selectedColor; ///< 选中颜色

    float m_titleY;      ///< 标题Y坐标
    float m_menuStartY;  ///< 菜单起始Y坐标
    float m_menuSpacing; ///< 菜单项间距

    bool m_mouseHovering; ///< 鼠标是否悬停在菜单项上

    sf::RenderWindow *m_window; ///< 游戏窗口指针
};
} // namespace SamuraiFight