#pragma once

#include "Scene.h"
#include "../entities/CharacterData.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

namespace SamuraiFight {

/**
 * @file CharacterSelectScene.h
 * @brief 角色选择场景
 */

/**
 * @struct PlayerSelection
 * @brief 玩家选择状态
 */
struct PlayerSelection {
    int characterIndex = -1;    ///< 选中的角色索引 (-1表示未选择)
    bool confirmed = false;     ///< 是否确认选择
};

/**
 * @class CharacterSelectScene
 * @brief 角色选择场景
 *
 * 显示可选角色列表，允许两个玩家选择角色
 * 当两个玩家都确认选择后进入战斗场景
 */
class CharacterSelectScene : public Scene {
public:
    /**
     * @brief 构造函数
     */
    CharacterSelectScene();

    /**
     * @brief 析构函数
     */
    ~CharacterSelectScene() override;

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
    void handleEvents(sf::RenderWindow& window) override;

    /**
     * @brief 更新场景逻辑
     *
     * @param deltaTime 固定时间步长
     */
    void update(float deltaTime) override;

    /**
     * @brief 渲染角色选择界面
     *
     * @param window 游戏窗口
     */
    void render(sf::RenderWindow& window) override;

private:
    /**
     * @brief 加载可用角色列表
     */
    void loadAvailableCharacters();

    /**
     * @brief 初始化UI元素
     */
    void initializeUI();

    /**
     * @brief 处理玩家1输入
     */
    void handlePlayer1Input();

    /**
     * @brief 处理玩家2输入
     */
    void handlePlayer2Input();

    /**
     * @brief 更新角色预览
     *
     * @param playerIndex 玩家索引
     */
    void updateCharacterPreview(int playerIndex);

    /**
     * @brief 检查是否可以开始游戏
     *
     * @return true 两个玩家都已确认
     * @return false 还有玩家未确认
     */
    bool canStartGame() const;

    /**
     * @brief 开始游戏
     */
    void startGame();

    /**
     * @brief 渲染角色卡片
     *
     * @param window 游戏窗口
     * @param characterIndex 角色索引
     * @param position 卡片位置
     * @param isSelected 是否被选中
     * @param playerIndex 选中该角色的玩家索引 (-1表示未选中)
     */
    void renderCharacterCard(sf::RenderWindow& window, int characterIndex,
                            const sf::Vector2f& position, bool isSelected, int playerIndex);

    /**
     * @brief 渲染玩家选择区域
     *
     * @param window 游戏窗口
     * @param playerIndex 玩家索引
     */
    void renderPlayerArea(sf::RenderWindow& window, int playerIndex);

    /**
     * @brief 渲染角色详细信息
     *
     * @param window 游戏窗口
     * @param data 角色数据
     * @param position 信息显示位置
     */
    void renderCharacterInfo(sf::RenderWindow& window, const CharacterData& data,
                            const sf::Vector2f& position);

    std::vector<CharacterData> m_availableCharacters;   ///< 可用角色列表
    std::vector<std::string> m_characterIds;            ///< 角色ID列表
    PlayerSelection m_player1Selection;                 ///< 玩家1选择状态
    PlayerSelection m_player2Selection;                 ///< 玩家2选择状态

    std::unique_ptr<sf::Text> m_titleText;              ///< 标题文本
    std::unique_ptr<sf::Text> m_instructionText;        ///< 提示文本
    std::unique_ptr<sf::Text> m_player1Label;           ///< 玩家1标签
    std::unique_ptr<sf::Text> m_player2Label;           ///< 玩家2标签

    sf::Color m_normalColor;    ///< 正常颜色
    sf::Color m_selectedColor;  ///< 选中颜色
    sf::Color m_player1Color;   ///< 玩家1颜色
    sf::Color m_player2Color;   ///< 玩家2颜色
    sf::Color m_confirmedColor; ///< 确认颜色

    float m_cardWidth;          ///< 角色卡片宽度
    float m_cardHeight;         ///< 角色卡片高度
    float m_cardSpacing;        ///< 卡片间距

    bool m_readyToStart;        ///< 是否准备好开始游戏
    float m_startDelay;         ///< 开始延迟计时器

    // 按键状态跟踪（防止按住连续触发）
    bool m_p1LeftPressed;   ///< 玩家1左键是否已按下
    bool m_p1RightPressed;  ///< 玩家1右键是否已按下
    bool m_p1ConfirmPressed;///< 玩家1确认键是否已按下
    bool m_p1CancelPressed; ///< 玩家1取消键是否已按下
    bool m_p2LeftPressed;   ///< 玩家2左键是否已按下
    bool m_p2RightPressed;  ///< 玩家2右键是否已按下
    bool m_p2ConfirmPressed;///< 玩家2确认键是否已按下
    bool m_p2CancelPressed; ///< 玩家2取消键是否已按下
};

} // namespace SamuraiFight
