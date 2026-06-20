#include "InputManager.h"
#include "../core/Game.h"
#include "../utils/JsonLoader.h"

#include <iostream>

namespace SamuraiFight {

InputManager::InputManager()
    : m_pausePressed(false) {
}

InputManager::~InputManager() {
}

bool InputManager::initialize() {
    // 尝试从配置管理器加载键位
    const auto& keyBindings = Game::getInstance().getConfigManager().getKeyBindings();

    if (!keyBindings.empty()) {
        loadKeyBindingsFromJson(keyBindings);
        std::cout << "InputManager: Loaded key bindings from config" << std::endl;
    } else {
        loadDefaultKeyBindings();
        std::cout << "InputManager: Using default key bindings" << std::endl;
    }

    return true;
}

void InputManager::update(sf::RenderWindow& /*window*/) {
    // 重置输入状态
    m_player1Input.reset();
    m_player2Input.reset();
    m_pausePressed = false;

    // 更新玩家1输入
    auto checkKey1 = [this](const std::string& action) -> bool {
        auto it = m_player1Keys.find(action);
        if (it != m_player1Keys.end()) {
            return sf::Keyboard::isKeyPressed(it->second);
        }
        return false;
    };

    m_player1Input.moveLeft = checkKey1("moveLeft");
    m_player1Input.moveRight = checkKey1("moveRight");
    m_player1Input.jump = checkKey1("jump");
    m_player1Input.crouch = checkKey1("crouch");
    m_player1Input.attackLight = checkKey1("attackLight");
    m_player1Input.attackMedium = checkKey1("attackMedium");
    m_player1Input.attackHeavy = checkKey1("attackHeavy");
    m_player1Input.attackSpecial = checkKey1("attackSpecial");
    m_player1Input.block = checkKey1("block");
    m_player1Input.roll = checkKey1("roll");

    // 更新玩家2输入
    auto checkKey2 = [this](const std::string& action) -> bool {
        auto it = m_player2Keys.find(action);
        if (it != m_player2Keys.end()) {
            return sf::Keyboard::isKeyPressed(it->second);
        }
        return false;
    };

    m_player2Input.moveLeft = checkKey2("moveLeft");
    m_player2Input.moveRight = checkKey2("moveRight");
    m_player2Input.jump = checkKey2("jump");
    m_player2Input.crouch = checkKey2("crouch");
    m_player2Input.attackLight = checkKey2("attackLight");
    m_player2Input.attackMedium = checkKey2("attackMedium");
    m_player2Input.attackHeavy = checkKey2("attackHeavy");
    m_player2Input.attackSpecial = checkKey2("attackSpecial");
    m_player2Input.block = checkKey2("block");
    m_player2Input.roll = checkKey2("roll");

    // 更新全局输入
    if (m_globalKeys.find("pause") != m_globalKeys.end()) {
        m_pausePressed = sf::Keyboard::isKeyPressed(m_globalKeys["pause"]);
    }
}

InputState InputManager::getPlayerInput(int playerIndex) const {
    if (playerIndex == 0) {
        return m_player1Input;
    } else {
        return m_player2Input;
    }
}

bool InputManager::isPausePressed() const {
    return m_pausePressed;
}

void InputManager::loadDefaultKeyBindings() {
    // 玩家1默认键位 (WASD + JKLI + O翻滚)
    m_player1Keys["moveLeft"] = sf::Keyboard::Key::A;
    m_player1Keys["moveRight"] = sf::Keyboard::Key::D;
    m_player1Keys["jump"] = sf::Keyboard::Key::W;
    m_player1Keys["crouch"] = sf::Keyboard::Key::S;
    m_player1Keys["attackLight"] = sf::Keyboard::Key::J;
    m_player1Keys["attackMedium"] = sf::Keyboard::Key::K;
    m_player1Keys["attackHeavy"] = sf::Keyboard::Key::L;
    m_player1Keys["attackSpecial"] = sf::Keyboard::Key::U;
    m_player1Keys["block"] = sf::Keyboard::Key::I;
    m_player1Keys["roll"] = sf::Keyboard::Key::O;

    // 玩家2默认键位 (方向键 + 小键盘)
    m_player2Keys["moveLeft"] = sf::Keyboard::Key::Left;
    m_player2Keys["moveRight"] = sf::Keyboard::Key::Right;
    m_player2Keys["jump"] = sf::Keyboard::Key::Up;
    m_player2Keys["crouch"] = sf::Keyboard::Key::Down;
    m_player2Keys["attackLight"] = sf::Keyboard::Key::Numpad1;
    m_player2Keys["attackMedium"] = sf::Keyboard::Key::Numpad2;
    m_player2Keys["attackHeavy"] = sf::Keyboard::Key::Numpad3;
    m_player2Keys["attackSpecial"] = sf::Keyboard::Key::Numpad4;
    m_player2Keys["block"] = sf::Keyboard::Key::Numpad5;
    m_player2Keys["roll"] = sf::Keyboard::Key::Numpad6;

    // 全局键位
    m_globalKeys["pause"] = sf::Keyboard::Key::Escape;
}

void InputManager::loadKeyBindingsFromJson(const nlohmann::json& json) {
    // 加载玩家1键位
    if (json.contains("player1")) {
        for (const auto& [action, keyName] : json["player1"].items()) {
            m_player1Keys[action] = stringToKey(keyName.get<std::string>());
        }
    }

    // 加载玩家2键位
    if (json.contains("player2")) {
        for (const auto& [action, keyName] : json["player2"].items()) {
            m_player2Keys[action] = stringToKey(keyName.get<std::string>());
        }
    }

    // 加载全局键位
    if (json.contains("global")) {
        for (const auto& [action, keyName] : json["global"].items()) {
            m_globalKeys[action] = stringToKey(keyName.get<std::string>());
        }
    }
}

bool InputManager::isKeyPressed(const std::string& keyName) const {
    auto it = m_player1Keys.find(keyName);
    if (it != m_player1Keys.end()) {
        return sf::Keyboard::isKeyPressed(it->second);
    }
    return false;
}

sf::Keyboard::Key InputManager::stringToKey(const std::string& keyName) const {
    // 字母键
    if (keyName.length() == 1) {
        char c = keyName[0];
        if (c >= 'A' && c <= 'Z') {
            return static_cast<sf::Keyboard::Key>(c - 'A');
        }
    }

    // 特殊键映射
    static const std::unordered_map<std::string, sf::Keyboard::Key> keyMap = {
        {"Left", sf::Keyboard::Key::Left},
        {"Right", sf::Keyboard::Key::Right},
        {"Up", sf::Keyboard::Key::Up},
        {"Down", sf::Keyboard::Key::Down},
        {"Escape", sf::Keyboard::Key::Escape},
        {"Space", sf::Keyboard::Key::Space},
        {"Enter", sf::Keyboard::Key::Enter},
        {"Numpad0", sf::Keyboard::Key::Numpad0},
        {"Numpad1", sf::Keyboard::Key::Numpad1},
        {"Numpad2", sf::Keyboard::Key::Numpad2},
        {"Numpad3", sf::Keyboard::Key::Numpad3},
        {"Numpad4", sf::Keyboard::Key::Numpad4},
        {"Numpad5", sf::Keyboard::Key::Numpad5},
        {"Numpad6", sf::Keyboard::Key::Numpad6},
        {"Numpad7", sf::Keyboard::Key::Numpad7},
        {"Numpad8", sf::Keyboard::Key::Numpad8},
        {"Numpad9", sf::Keyboard::Key::Numpad9}
    };

    auto it = keyMap.find(keyName);
    if (it != keyMap.end()) {
        return it->second;
    }

    // 默认返回未知键
    return sf::Keyboard::Key::Unknown;
}

} // namespace SamuraiFight