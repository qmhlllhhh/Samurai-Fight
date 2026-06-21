#pragma once

#include "InputCommand.h"
#include "InputBuffer.h"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>

namespace SamuraiFight {

/**
 * @file InputManager.h
 * @brief 输入管理器
 */

/**
 * @class InputManager
 * @brief 管理玩家输入
 *
 * 处理键盘输入，区分玩家1和玩家2
 */
class InputManager {
public:
    /**
     * @brief 构造函数
     */
    InputManager();

    /**
     * @brief 析构函数
     */
    ~InputManager();

    /**
     * @brief 初始化输入管理器
     *
     * 加载键位配置
     *
     * @return true 初始化成功
     * @return false 初始化失败
     */
    bool initialize();

    /**
     * @brief 更新输入状态
     *
     * @param window 游戏窗口
     */
    void update(sf::RenderWindow& window);

    /**
     * @brief 获取玩家输入状态
     *
     * @param playerIndex 玩家索引 (0或1)
     * @return InputState 输入状态
     */
    InputState getPlayerInput(int playerIndex) const;

    /**
     * @brief 检查暂停键是否按下
     *
     * @return true 暂停键按下
     * @return false 暂停键未按下
     */
    bool isPausePressed() const;

    /**
     * @brief 将 SFML 键码转为键名字符串（stringToKey 的逆，供保存/显示用）
     *
     * @param key SFML 键码
     * @return std::string 键名（如 "A"、"Left"、"Numpad1"）
     */
    static std::string keyToString(sf::Keyboard::Key key);

    /**
     * @brief 将物理扫描码转为键名字符串（供键位重映射捕获用）
     *
     * 用 Scancode 而非 Key：SFML 3.0 在某些键盘布局下事件 code
     * 对字母/数字返回 Unknown(-1)，而 scancode 总是有效。
     *
     * @param scancode 物理扫描码
     * @return std::string 键名
     */
    static std::string scancodeToString(sf::Keyboard::Scancode scancode);

private:
    /**
     * @brief 加载默认键位
     */
    void loadDefaultKeyBindings();

    /**
     * @brief 从JSON加载键位
     *
     * @param json JSON配置
     */
    void loadKeyBindingsFromJson(const nlohmann::json& json);

    /**
     * @brief 检查按键是否按下
     *
     * @param keyName 键名
     * @return true 按下
     * @return false 未按下
     */
    bool isKeyPressed(const std::string& keyName) const;

    /**
     * @brief 将键名字符串转换为SFML键码
     *
     * @param keyName 键名
     * @return sf::Keyboard::Key 键码
     */
    sf::Keyboard::Key stringToKey(const std::string& keyName) const;

    /**
     * @brief 更新单个玩家的输入
     *
     * @param keyBindings 键位映射
     * @param buffer 输入缓冲
     * @param keyStates 按键状态追踪
     * @param inputState 输出输入状态
     * @param playerId 玩家ID (1或2)
     */
    void updatePlayerInput(
        std::unordered_map<std::string, sf::Keyboard::Key>& keyBindings,
        InputBuffer& buffer,
        std::unordered_map<std::string, bool>& keyStates,
        InputState& inputState,
        int playerId);

    // 玩家1键位
    std::unordered_map<std::string, sf::Keyboard::Key> m_player1Keys;

    // 玩家2键位
    std::unordered_map<std::string, sf::Keyboard::Key> m_player2Keys;

    // 全局键位
    std::unordered_map<std::string, sf::Keyboard::Key> m_globalKeys;

    // 输入状态
    InputState m_player1Input;
    InputState m_player2Input;
    bool m_pausePressed;

    // 输入缓冲
    InputBuffer m_player1Buffer;
    InputBuffer m_player2Buffer;
    int m_currentFrame;

    // 按键状态追踪（用于检测按下/释放事件）
    std::unordered_map<std::string, bool> m_player1KeyStates;
    std::unordered_map<std::string, bool> m_player2KeyStates;
};

} // namespace SamuraiFight