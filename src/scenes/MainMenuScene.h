#pragma once

#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

namespace SamuraiFight {

/**
 * @file MainMenuScene.h
 * @brief 主菜单场景
 */

/**
 * @enum MenuItem
 * @brief 菜单项标识
 */
enum class MenuItem {
    StartGame, ///< 开始游戏
    Settings,  ///< 设置
    Exit       ///< 退出游戏
};

/**
 * @class MainMenuScene
 * @brief 主菜单场景
 *
 * 显示游戏主菜单，包括：
 * - 游戏标题
 * - 菜单选项（开始游戏、设置、退出）
 * - 键盘/鼠标输入处理
 */
class MainMenuScene : public Scene {
  public:
    /**
     * @brief 构造函数
     */
    MainMenuScene();

    /**
     * @brief 析构函数
     */
    ~MainMenuScene() override;

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
     * @brief 更新场景逻辑
     *
     * @param deltaTime 固定时间步长
     */
    void update(float deltaTime) override;

    /**
     * @brief 渲染主菜单
     *
     * @param window 游戏窗口
     */
    void render(sf::RenderWindow &window) override;

  private:
    /**
     * @brief 初始化菜单项
     */
    void initMenuItems();

    /**
     * @brief 更新菜单项选择
     *
     * @param mousePos 鼠标位置
     */
    void updateMenuSelection(const sf::Vector2f &mousePos);

    /**
     * @brief 处理菜单项点击
     */
    void handleMenuClick();

    /**
     * @brief 选择上一个菜单项
     */
    void selectPreviousItem();

    /**
     * @brief 选择下一个菜单项
     */
    void selectNextItem();

    /**
     * @brief 执行当前选中的菜单项
     */
    void executeCurrentItem();

    std::unique_ptr<sf::Text> m_titleText;              ///< 游戏标题文本
    std::vector<std::unique_ptr<sf::Text>> m_menuItems; ///< 菜单项文本
    std::vector<MenuItem> m_menuActions;                ///< 菜单项动作

    int m_selectedIndex;       ///< 当前选中项索引
    sf::Color m_normalColor;   ///< 正常颜色
    sf::Color m_selectedColor; ///< 选中颜色

    float m_titleY;      ///< 标题Y坐标
    float m_menuStartY;  ///< 菜单起始Y坐标
    float m_menuSpacing; ///< 菜单项间距

    bool m_mouseHovering;       ///< 鼠标是否悬停在菜单项上
    sf::RenderWindow *m_window; ///< 窗口指针（用于关闭窗口）
};

} // namespace SamuraiFight