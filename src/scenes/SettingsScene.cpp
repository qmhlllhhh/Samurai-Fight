#include "SettingsScene.h"
#include "../core/Constants.h"
#include "../core/Game.h"
#include "../input/InputManager.h"
#include "../managers/AudioManager.h"
#include "../managers/ConfigManager.h"
#include "../managers/ResourceManager.h"
#include <cmath>
#include <iostream>
#include <string>

namespace SamuraiFight {

SettingsScene::SettingsScene()
    : m_selectedIndex(0), m_normalColor(sf::Color::White), m_selectedColor(sf::Color(255, 200, 0)), m_titleY(100.0f), m_menuStartY(240.0f), m_menuSpacing(64.0f) {
    // 填充键位重映射动作（P1 + P2 各 8 个）
    // 注：crouch（下蹲）与 attackSpecial（特殊攻击）已在同学的重构中从
    // Character::handleInput 移除，故不提供这两个动作的重映射。
    std::vector<std::string> actions = {
        "moveLeft", "moveRight", "jump",
        "attackLight", "attackMedium", "attackHeavy",
        "block", "roll"};
    for (int p = 0; p < 2; ++p) {
        for (const auto &a : actions) {
            m_rebindActions.push_back({p, a});
        }
    }
}

SettingsScene::~SettingsScene() {
}

void SettingsScene::onEnter() {
    std::cout << "SettingsScene: Entered" << std::endl;

    try {
        const sf::Font &font = ResourceManager::getInstance().getFont("defaultBold");

        m_titleText = std::make_unique<sf::Text>(font);
        m_titleText->setString("Settings");
        m_titleText->setCharacterSize(64);
        m_titleText->setFillColor(sf::Color::White);
        sf::FloatRect tb = m_titleText->getLocalBounds();
        m_titleText->setPosition(sf::Vector2f((WINDOW_WIDTH - tb.size.x) / 2.0f, m_titleY));

        initMenuItems();
    } catch (const std::exception &e) {
        std::cerr << "SettingsScene: Error initializing - " << e.what() << std::endl;
    }
}

void SettingsScene::onExit() {
    std::cout << "SettingsScene: Exited" << std::endl;
}

void SettingsScene::initMenuItems() {
    const sf::Font &font = ResourceManager::getInstance().getDefaultFont();

    m_itemTexts.clear();
    m_itemActions.clear();

    std::vector<SettingsItem> actions = {
        SettingsItem::MasterVolume,
        SettingsItem::BGMVolume,
        SettingsItem::SFXVolume,
        SettingsItem::RebindKeys,
        SettingsItem::Back};

    for (size_t i = 0; i < actions.size(); ++i) {
        auto text = std::make_unique<sf::Text>(font);
        text->setCharacterSize(36);
        m_itemTexts.push_back(std::move(text));
        m_itemActions.push_back(actions[i]);
    }

    m_selectedIndex = 0;
    refreshItemTexts();
}

void SettingsScene::refreshItemTexts() {
    ConfigManager &cfg = Game::getInstance().getConfigManager();

    for (size_t i = 0; i < m_itemTexts.size(); ++i) {
        if (!m_itemTexts[i])
            continue;

        SettingsItem item = m_itemActions[i];
        std::string text;
        switch (item) {
        case SettingsItem::MasterVolume:
            text = "Master Volume:  " + std::to_string(static_cast<int>(cfg.getMasterVolume() * 100)) + "%";
            break;
        case SettingsItem::BGMVolume:
            text = "BGM Volume:  " + std::to_string(static_cast<int>(cfg.getBGMVolume() * 100)) + "%";
            break;
        case SettingsItem::SFXVolume:
            text = "SFX Volume:  " + std::to_string(static_cast<int>(cfg.getSFXVolume() * 100)) + "%";
            break;
        case SettingsItem::RebindKeys:
            text = "Rebind Keys";
            break;
        case SettingsItem::Back:
            text = "Back (Save)";
            break;
        }

        m_itemTexts[i]->setString(text);
        sf::FloatRect bounds = m_itemTexts[i]->getLocalBounds();
        m_itemTexts[i]->setPosition(sf::Vector2f(
            (WINDOW_WIDTH - bounds.size.x) / 2.0f,
            m_menuStartY + i * m_menuSpacing));
        m_itemTexts[i]->setFillColor((static_cast<int>(i) == m_selectedIndex) ? m_selectedColor : m_normalColor);
    }
}

void SettingsScene::handleEvents(sf::RenderWindow &window) {
    // 鼠标悬停选中（非重映射模式时每帧更新）
    if (m_rebindStep < 0) {
        sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
        updateSelection(mousePos);
    }

    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        // 键位重映射向导：用 scancode 捕获（字母/数字均有效），Tab 前进，Esc 取消
        if (m_rebindStep >= 0) {
            if (const auto *k = event->getIf<sf::Event::KeyPressed>()) {
                sf::Keyboard::Scancode sc = k->scancode;
                if (sc == sf::Keyboard::Scancode::Escape) {
                    AudioManager::getInstance().playSound("select");
                    m_rebindStep = -1;
                } else if (sc == sf::Keyboard::Scancode::Tab) {
                    // Tab 前进到下一个动作（无论当前是否改过绑定）
                    m_rebindStep++;
                    if (m_rebindStep >= static_cast<int>(m_rebindActions.size())) {
                        AudioManager::getInstance().playSound("select");
                        m_rebindStep = -1;
                    } else {
                        AudioManager::getInstance().playSound("cursor");
                    }
                } else if (sc != sf::Keyboard::Scancode::Unknown) {
                    handleRebindKey(sc); // 仅设置当前绑定，不自动前进
                }
            }
            continue;
        }

        // 键盘
        if (const auto *k = event->getIf<sf::Event::KeyPressed>()) {
            switch (k->code) {
            case sf::Keyboard::Key::Up:
                m_mouseHovering = false;
                selectPreviousItem();
                break;
            case sf::Keyboard::Key::Down:
                m_mouseHovering = false;
                selectNextItem();
                break;
            case sf::Keyboard::Key::Left:
                adjustCurrentVolume(-0.1f);
                break;
            case sf::Keyboard::Key::Right:
                adjustCurrentVolume(0.1f);
                break;
            case sf::Keyboard::Key::Enter:
                confirmSelection();
                break;
            case sf::Keyboard::Key::Escape:
                backAndSave();
                break;
            default:
                break;
            }
        }

        // 鼠标左键 = 点击操作（音量项左减右加 / Back·Rebind 确认）
        if (const auto *mb = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mb->button == sf::Mouse::Button::Left) {
                sf::Vector2f clickPos = sf::Vector2f(sf::Mouse::getPosition(window));
                handleMouseClick(clickPos);
            }
        }

        // 鼠标滚轮 = 调音量
        if (const auto *wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
            if (wheel->delta > 0) {
                adjustCurrentVolume(0.1f);
            } else if (wheel->delta < 0) {
                adjustCurrentVolume(-0.1f);
            }
        }
    }
}

void SettingsScene::update(float /*deltaTime*/) {
    // 无复杂更新逻辑
}

void SettingsScene::render(sf::RenderWindow &window) {
    // 全屏背景，遮挡下层场景（主菜单/战斗）
    sf::RectangleShape bg(sf::Vector2f(static_cast<float>(WINDOW_WIDTH),
                                       static_cast<float>(WINDOW_HEIGHT)));
    bg.setFillColor(sf::Color(20, 20, 30));
    bg.setPosition(sf::Vector2f(0.0f, 0.0f));
    window.draw(bg);

    // 键位重映射向导界面
    if (m_rebindStep >= 0) {
        try {
            const sf::Font &font = ResourceManager::getInstance().getDefaultFont();

            sf::Text title(font);
            title.setString("Rebinding Keys  (" + std::to_string(m_rebindStep + 1) + "/" +
                            std::to_string(m_rebindActions.size()) + ")");
            title.setCharacterSize(44);
            title.setFillColor(sf::Color::White);
            sf::FloatRect tb = title.getLocalBounds();
            title.setPosition(sf::Vector2f((WINDOW_WIDTH - tb.size.x) / 2.0f, 220.0f));
            window.draw(title);

            sf::Text prompt(font);
            prompt.setString(rebindPromptText());
            prompt.setCharacterSize(32);
            prompt.setFillColor(sf::Color(255, 200, 0));
            sf::FloatRect pb = prompt.getLocalBounds();
            prompt.setPosition(sf::Vector2f((WINDOW_WIDTH - pb.size.x) / 2.0f, 340.0f));
            window.draw(prompt);
        } catch (const std::exception &e) {
            std::cerr << "SettingsScene: Error rendering rebind - " << e.what() << std::endl;
        }
        return;
    }

    if (m_titleText) {
        window.draw(*m_titleText);
    }

    for (const auto &t : m_itemTexts) {
        if (t)
            window.draw(*t);
    }

    try {
        const sf::Font &font = ResourceManager::getInstance().getDefaultFont();
        sf::Text hint(font);
        hint.setString("Up/Down or Mouse: select   Left/Right or Wheel: volume\nEnter/Click: confirm   Esc: back & save");
        hint.setCharacterSize(20);
        hint.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect hb = hint.getLocalBounds();
        hint.setPosition(sf::Vector2f((WINDOW_WIDTH - hb.size.x) / 2.0f, WINDOW_HEIGHT - 70.0f));
        window.draw(hint);
    } catch (const std::exception &e) {
        std::cerr << "SettingsScene: Error rendering hint - " << e.what() << std::endl;
    }
}

void SettingsScene::selectPreviousItem() {
    AudioManager::getInstance().playSound("cursor");
    m_selectedIndex--;
    if (m_selectedIndex < 0)
        m_selectedIndex = static_cast<int>(m_itemTexts.size()) - 1;
    refreshItemTexts();
}

void SettingsScene::selectNextItem() {
    AudioManager::getInstance().playSound("cursor");
    m_selectedIndex++;
    if (m_selectedIndex >= static_cast<int>(m_itemTexts.size()))
        m_selectedIndex = 0;
    refreshItemTexts();
}

void SettingsScene::adjustCurrentVolume(float delta) {
    if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_itemActions.size()))
        return;
    SettingsItem item = m_itemActions[m_selectedIndex];
    if (item == SettingsItem::Back || item == SettingsItem::RebindKeys)
        return;

    AudioManager::getInstance().playSound("select");

    ConfigManager &cfg = Game::getInstance().getConfigManager();
    float current = 0.0f;
    switch (item) {
    case SettingsItem::MasterVolume:
        current = cfg.getMasterVolume();
        break;
    case SettingsItem::BGMVolume:
        current = cfg.getBGMVolume();
        break;
    case SettingsItem::SFXVolume:
        current = cfg.getSFXVolume();
        break;
    default:
        return;
    }

    // 用整数百分比步进，避免浮点累加误差导致步进不均（如 79%/81%）
    int currentPercent = static_cast<int>(std::round(current * 100.0f));
    int newPercent = currentPercent + static_cast<int>(std::round(delta * 100.0f));
    if (newPercent < 0)
        newPercent = 0;
    if (newPercent > 100)
        newPercent = 100;
    if (newPercent == currentPercent)
        return;

    float v = static_cast<float>(newPercent) / 100.0f;
    applyVolume(item, v);
    refreshItemTexts();
}

void SettingsScene::applyVolume(SettingsItem item, float v) {
    ConfigManager &cfg = Game::getInstance().getConfigManager();
    switch (item) {
    case SettingsItem::MasterVolume:
        cfg.setMasterVolume(v);
        AudioManager::getInstance().setMasterVolume(v);
        break;
    case SettingsItem::BGMVolume:
        cfg.setBGMVolume(v);
        AudioManager::getInstance().setBGMVolume(v);
        break;
    case SettingsItem::SFXVolume:
        cfg.setSFXVolume(v);
        AudioManager::getInstance().setSFXVolume(v);
        break;
    default:
        break;
    }
}

void SettingsScene::backAndSave() {
    AudioManager::getInstance().playSound("select");
    ConfigManager &cfg = Game::getInstance().getConfigManager();
    cfg.saveGameConfig("assets/config/game_config.json");
    cfg.saveKeyBindings("assets/config/keybindings.json");
    std::cout << "SettingsScene: Config & keybindings saved, returning" << std::endl;
    m_popSceneCount = 1; // pop 本场景，返回上层（主菜单 / 暂停）
}

void SettingsScene::enterRebindMode() {
    AudioManager::getInstance().playSound("select");
    m_rebindStep = 0;
}

void SettingsScene::handleRebindKey(sf::Keyboard::Scancode scancode) {
    if (m_rebindStep < 0 || m_rebindStep >= static_cast<int>(m_rebindActions.size()))
        return;

    auto [player, action] = m_rebindActions[m_rebindStep];
    std::string keyName = InputManager::scancodeToString(scancode);
    if (keyName == "Unknown")
        return; // 未知/不支持的键忽略

    // 仅设置当前动作的绑定，不自动前进（按 Tab 才前进）
    Game::getInstance().getConfigManager().setKeyBinding(player, action, keyName);
    std::cout << "SettingsScene: Set P" << (player + 1) << " " << action << " -> " << keyName << std::endl;
}

std::string SettingsScene::rebindPromptText() const {
    if (m_rebindStep < 0 || m_rebindStep >= static_cast<int>(m_rebindActions.size()))
        return "";
    auto [player, action] = m_rebindActions[m_rebindStep];

    // 显示当前绑定（设置后实时更新，便于确认按键是否生效）
    const auto &kb = Game::getInstance().getConfigManager().getKeyBindings();
    std::string section = (player == 0) ? "player1" : "player2";
    std::string current = "Unbound";
    if (kb.contains(section) && kb[section].contains(action)) {
        current = kb[section][action].get<std::string>();
    }

    return "Player " + std::to_string(player + 1) + "  " + action +
           "   (current: " + current + ")" +
           "\npress a key to set   Tab: next   Esc: cancel";
}

void SettingsScene::updateSelection(const sf::Vector2f &mousePos) {
    bool foundHover = false;
    for (size_t i = 0; i < m_itemTexts.size(); ++i) {
        if (!m_itemTexts[i])
            continue;
        sf::FloatRect bounds = m_itemTexts[i]->getGlobalBounds();
        if (mousePos.x >= 0 && mousePos.y >= 0 && bounds.contains(mousePos)) {
            if (m_selectedIndex != static_cast<int>(i)) {
                m_selectedIndex = static_cast<int>(i);
                AudioManager::getInstance().playSound("cursor");
                refreshItemTexts();
            }
            m_mouseHovering = true;
            foundHover = true;
        }
    }
    if (!foundHover) {
        m_mouseHovering = false;
    }
}

void SettingsScene::confirmSelection() {
    if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_itemActions.size()))
        return;
    SettingsItem item = m_itemActions[m_selectedIndex];
    if (item == SettingsItem::Back) {
        backAndSave();
    } else if (item == SettingsItem::RebindKeys) {
        enterRebindMode();
    }
}

void SettingsScene::handleMouseClick(const sf::Vector2f &pos) {
    for (size_t i = 0; i < m_itemTexts.size(); ++i) {
        if (!m_itemTexts[i])
            continue;
        sf::FloatRect bounds = m_itemTexts[i]->getGlobalBounds();
        if (bounds.contains(pos)) {
            m_selectedIndex = static_cast<int>(i);
            SettingsItem item = m_itemActions[i];
            if (item == SettingsItem::Back) {
                backAndSave();
            } else if (item == SettingsItem::RebindKeys) {
                enterRebindMode();
            } else {
                // 音量项：点击左半减、右半加（模拟滑块）
                float centerX = bounds.position.x + bounds.size.x / 2.0f;
                adjustCurrentVolume(pos.x < centerX ? -0.1f : 0.1f);
            }
            refreshItemTexts();
            return;
        }
    }
}

} // namespace SamuraiFight
