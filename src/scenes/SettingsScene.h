#pragma once

#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <utility>
#include <vector>

namespace SamuraiFight {

/**
 * @file SettingsScene.h
 * @brief 设置场景
 */

/**
 * @enum SettingsItem
 * @brief 设置菜单项
 */
enum class SettingsItem {
    MasterVolume,  ///< 主音量
    BGMVolume,     ///< 背景音乐音量
    SFXVolume,     ///< 音效音量
    RebindKeys,    ///< 键位重映射
    Back           ///< 返回（保存）
};

/**
 * @class SettingsScene
 * @brief 设置场景
 *
 * 音量调节 + 键位重映射。支持键盘与鼠标：
 * - 键盘：Up/Down 选择，Left/Right 调音量，Enter 确认，Esc 返回保存
 * - 鼠标：移动悬停选中，左键确认，滚轮调音量
 *
 * 键位重映射为向导式：Tab 跳过当前动作，Esc 取消。
 */
class SettingsScene : public Scene {
public:
    SettingsScene();
    ~SettingsScene() override;

    void onEnter() override;
    void onExit() override;
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void initMenuItems();
    void refreshItemTexts();                ///< 刷新各项文本（含音量百分比）与选中颜色
    void selectPreviousItem();
    void selectNextItem();
    void adjustCurrentVolume(float delta);  ///< 调当前音量项（Left/Right 或滚轮）
    void applyVolume(SettingsItem item, float v);  ///< 写 ConfigManager（+ AudioManager 预留）
    void backAndSave();                     ///< 保存配置（音量+键位）到 JSON 并返回上层
    void enterRebindMode();                 ///< 进入键位重映射向导
    void handleRebindKey(sf::Keyboard::Scancode scancode);  ///< 处理重映射捕获的按键（用 scancode）
    std::string rebindPromptText() const;   ///< 当前重映射步骤的提示文本
    void updateSelection(const sf::Vector2f& mousePos);  ///< 鼠标悬停选中
    void confirmSelection();                ///< 确认当前选中（Enter / 鼠标左键）

    std::unique_ptr<sf::Text> m_titleText;
    std::vector<std::unique_ptr<sf::Text>> m_itemTexts;
    std::vector<SettingsItem> m_itemActions;

    int m_selectedIndex;
    sf::Color m_normalColor;
    sf::Color m_selectedColor;
    bool m_mouseHovering = false;  ///< 是否鼠标悬停模式

    float m_titleY;
    float m_menuStartY;
    float m_menuSpacing;

    int m_rebindStep = -1;  ///< 键位重映射向导步骤（-1=关闭）
    std::vector<std::pair<int, std::string>> m_rebindActions;  ///< (player, action)
};

} // namespace SamuraiFight
