#include "CharacterSelectScene.h"
#include "../core/Constants.h"
#include "../entities/CharacterFactory.h"
#include "../managers/AudioManager.h"
#include "../managers/ResourceManager.h"
#include "BattleScene.h"
#include <filesystem>
#include <iostream>

namespace SamuraiFight {

CharacterSelectScene::CharacterSelectScene()
    : m_normalColor(sf::Color::White), m_selectedColor(sf::Color(255, 200, 0)) // 金色
      ,
      m_player1Color(sf::Color(100, 150, 255)) // 蓝色
      ,
      m_player2Color(sf::Color(255, 100, 100)) // 红色
      ,
      m_confirmedColor(sf::Color(100, 255, 100)) // 绿色
      ,
      m_cardWidth(200.0f), m_cardHeight(280.0f), m_cardSpacing(30.0f), m_readyToStart(false), m_startDelay(0.0f), m_p1LeftPressed(false), m_p1RightPressed(false), m_p1ConfirmPressed(false), m_p1CancelPressed(false), m_p2LeftPressed(false), m_p2RightPressed(false), m_p2ConfirmPressed(false), m_p2CancelPressed(false) {
}

CharacterSelectScene::~CharacterSelectScene() {
}

void CharacterSelectScene::onEnter() {
    std::cout << "CharacterSelectScene: Entered" << std::endl;

    // 加载可用角色
    loadAvailableCharacters();

    // 初始化UI
    initializeUI();
}

void CharacterSelectScene::onExit() {
    std::cout << "CharacterSelectScene: Exited" << std::endl;
}

void CharacterSelectScene::loadAvailableCharacters() {
    m_availableCharacters.clear();
    m_characterIds.clear();

    // 扫描 assets/characters 目录下的所有角色
    std::string charactersPath = "assets/characters";

    try {
        for (const auto &entry : std::filesystem::directory_iterator(charactersPath)) {
            if (entry.is_directory()) {
                std::string characterId = entry.path().filename().string();
                std::string configPath = entry.path().string() + "/config.json";

                // 尝试加载角色数据
                CharacterData data;
                if (data.loadFromJson(configPath)) {
                    m_availableCharacters.push_back(data);
                    m_characterIds.push_back(characterId);
                    std::cout << "CharacterSelectScene: Loaded character '" << characterId
                              << "' (" << data.name << ")" << std::endl;
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "CharacterSelectScene: Error scanning characters directory - " << e.what() << std::endl;
    }

    // 如果没有找到角色，输出警告
    if (m_availableCharacters.empty()) {
        std::cerr << "CharacterSelectScene: No characters found!" << std::endl;
    }

    // 默认选择第一个角色
    if (!m_availableCharacters.empty()) {
        m_player1Selection.characterIndex = 0;
        m_player2Selection.characterIndex = 0;
    }
}

void CharacterSelectScene::initializeUI() {
    try {
        const sf::Font &font = ResourceManager::getInstance().getFont("defaultBold");

        // 设置标题
        m_titleText = std::make_unique<sf::Text>(font);
        m_titleText->setString("Select Your Character");
        m_titleText->setCharacterSize(56);
        m_titleText->setFillColor(sf::Color::White);

        sf::FloatRect titleBounds = m_titleText->getLocalBounds();
        m_titleText->setPosition(sf::Vector2f(
            (WINDOW_WIDTH - titleBounds.size.x) / 2.0f,
            30.0f));

        // 设置提示文本
        const sf::Font &normalFont = ResourceManager::getInstance().getDefaultFont();
        m_instructionText = std::make_unique<sf::Text>(normalFont);
        m_instructionText->setString(
            "Player 1: A/D to select, W to confirm, S to cancel\n"
            "Player 2: Left/Right to select, Up to confirm, Down to cancel");
        m_instructionText->setCharacterSize(20);
        m_instructionText->setFillColor(sf::Color(150, 150, 150));

        sf::FloatRect instructBounds = m_instructionText->getLocalBounds();
        m_instructionText->setPosition(sf::Vector2f(
            (WINDOW_WIDTH - instructBounds.size.x) / 2.0f,
            WINDOW_HEIGHT - 60.0f));

        // 设置玩家标签
        m_player1Label = std::make_unique<sf::Text>(font);
        m_player1Label->setString("Player 1");
        m_player1Label->setCharacterSize(32);
        m_player1Label->setFillColor(m_player1Color);
        m_player1Label->setPosition(sf::Vector2f(100.0f, 120.0f));

        m_player2Label = std::make_unique<sf::Text>(font);
        m_player2Label->setString("Player 2");
        m_player2Label->setCharacterSize(32);
        m_player2Label->setFillColor(m_player2Color);
        m_player2Label->setPosition(sf::Vector2f(WINDOW_WIDTH - 300.0f, 120.0f));

    } catch (const std::exception &e) {
        std::cerr << "CharacterSelectScene: Error initializing UI - " << e.what() << std::endl;
    }
}

void CharacterSelectScene::handleEvents(sf::RenderWindow &window) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            // ESC返回主菜单
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_popSceneCount = 1;
                return;
            }
        }
    }

    // 处理玩家输入
    handlePlayer1Input();
    handlePlayer2Input();

    // 检查是否可以开始游戏
    if (canStartGame() && !m_readyToStart) {
        m_readyToStart = true;
        m_startDelay = 0.0f;
    }
}

void CharacterSelectScene::handlePlayer1Input() {
    // S 取消确认（优先处理）
    bool sPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
    if (sPressed && !m_p1CancelPressed && m_player1Selection.confirmed) {
        AudioManager::getInstance().playSound("cancel");
        m_player1Selection.confirmed = false;
        m_readyToStart = false; // 重置准备状态
        std::cout << "CharacterSelectScene: Player 1 cancelled selection" << std::endl;
    }
    m_p1CancelPressed = sPressed;

    if (m_player1Selection.confirmed) {
        return; // 已确认，不能再改变选择
    }

    // A 选择角色（只在按键刚按下时触发）
    bool aPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    if (aPressed && !m_p1LeftPressed) {
        AudioManager::getInstance().playSound("cursor");
        m_player1Selection.characterIndex--;
        if (m_player1Selection.characterIndex < 0) {
            m_player1Selection.characterIndex = static_cast<int>(m_availableCharacters.size()) - 1;
        }
    }
    m_p1LeftPressed = aPressed;

    // D 选择角色
    bool dPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
    if (dPressed && !m_p1RightPressed) {
        AudioManager::getInstance().playSound("cursor");
        m_player1Selection.characterIndex++;
        if (m_player1Selection.characterIndex >= static_cast<int>(m_availableCharacters.size())) {
            m_player1Selection.characterIndex = 0;
        }
    }
    m_p1RightPressed = dPressed;

    // W 确认选择
    bool wPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
    if (wPressed && !m_p1ConfirmPressed) {
        AudioManager::getInstance().playSound("select");
        m_player1Selection.confirmed = true;
        std::cout << "CharacterSelectScene: Player 1 confirmed '"
                  << m_characterIds[m_player1Selection.characterIndex] << "'" << std::endl;
    }
    m_p1ConfirmPressed = wPressed;
}

void CharacterSelectScene::handlePlayer2Input() {
    // Down 取消确认（优先处理）
    bool downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);
    if (downPressed && !m_p2CancelPressed && m_player2Selection.confirmed) {
        AudioManager::getInstance().playSound("cancel");
        m_player2Selection.confirmed = false;
        m_readyToStart = false; // 重置准备状态
        std::cout << "CharacterSelectScene: Player 2 cancelled selection" << std::endl;
    }
    m_p2CancelPressed = downPressed;

    if (m_player2Selection.confirmed) {
        return; // 已确认，不能再改变选择
    }

    // Left 选择角色
    bool leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    if (leftPressed && !m_p2LeftPressed) {
        AudioManager::getInstance().playSound("cursor");
        m_player2Selection.characterIndex--;
        if (m_player2Selection.characterIndex < 0) {
            m_player2Selection.characterIndex = static_cast<int>(m_availableCharacters.size()) - 1;
        }
    }
    m_p2LeftPressed = leftPressed;

    // Right 选择角色
    bool rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
    if (rightPressed && !m_p2RightPressed) {
        AudioManager::getInstance().playSound("cursor");
        m_player2Selection.characterIndex++;
        if (m_player2Selection.characterIndex >= static_cast<int>(m_availableCharacters.size())) {
            m_player2Selection.characterIndex = 0;
        }
    }
    m_p2RightPressed = rightPressed;

    // Up 确认选择
    bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
    if (upPressed && !m_p2ConfirmPressed) {
        AudioManager::getInstance().playSound("select");
        m_player2Selection.confirmed = true;
        std::cout << "CharacterSelectScene: Player 2 confirmed '"
                  << m_characterIds[m_player2Selection.characterIndex] << "'" << std::endl;
    }
    m_p2ConfirmPressed = upPressed;
}

void CharacterSelectScene::update(float deltaTime) {
    if (m_readyToStart) {
        m_startDelay += deltaTime;
        // 延迟0.5秒后开始游戏
        if (m_startDelay >= 0.5f) {
            startGame();
        }
    }
}

bool CharacterSelectScene::canStartGame() const {
    return m_player1Selection.confirmed && m_player2Selection.confirmed;
}

void CharacterSelectScene::startGame() {
    std::cout << "CharacterSelectScene: Starting game with "
              << m_characterIds[m_player1Selection.characterIndex] << " vs "
              << m_characterIds[m_player2Selection.characterIndex] << std::endl;

    // 创建战斗场景，传递选择的角色ID
    m_nextScene = std::make_unique<BattleScene>(
        m_characterIds[m_player1Selection.characterIndex],
        m_characterIds[m_player2Selection.characterIndex]);
    m_popSceneCount = 1;
}

void CharacterSelectScene::render(sf::RenderWindow &window) {
    // 绘制标题
    if (m_titleText) {
        window.draw(*m_titleText);
    }

    // 绘制玩家区域
    renderPlayerArea(window, 0);
    renderPlayerArea(window, 1);

    // 绘制角色选择区域（中间）
    float startX = (WINDOW_WIDTH - (m_availableCharacters.size() * m_cardWidth +
                                    (m_availableCharacters.size() - 1) * m_cardSpacing)) /
                   2.0f;
    float cardY = 300.0f;

    for (size_t i = 0; i < m_availableCharacters.size(); ++i) {
        float x = startX + i * (m_cardWidth + m_cardSpacing);

        // 检查是否被选中
        bool isSelectedByP1 = (m_player1Selection.characterIndex == static_cast<int>(i));
        bool isSelectedByP2 = (m_player2Selection.characterIndex == static_cast<int>(i));

        // 渲染角色卡片
        int selectedBy = -1;
        if (isSelectedByP1 && isSelectedByP2) {
            selectedBy = 2; // 两个玩家都选中
        } else if (isSelectedByP1) {
            selectedBy = 0;
        } else if (isSelectedByP2) {
            selectedBy = 1;
        }

        renderCharacterCard(window, static_cast<int>(i), sf::Vector2f(x, cardY),
                            (selectedBy >= 0), selectedBy);
    }

    // 绘制提示文本
    if (m_instructionText) {
        window.draw(*m_instructionText);
    }

    // 如果准备好开始，显示提示
    if (m_readyToStart) {
        const sf::Font &font = ResourceManager::getInstance().getDefaultFont();
        sf::Text startText(font);
        startText.setString("Starting Battle...");
        startText.setCharacterSize(40);
        startText.setFillColor(m_confirmedColor);

        sf::FloatRect bounds = startText.getLocalBounds();
        startText.setPosition(sf::Vector2f(
            (WINDOW_WIDTH - bounds.size.x) / 2.0f,
            WINDOW_HEIGHT - 120.0f));

        window.draw(startText);
    }
}

void CharacterSelectScene::renderCharacterCard(sf::RenderWindow &window, int characterIndex,
                                               const sf::Vector2f &position, bool isSelected,
                                               int playerIndex) {
    if (characterIndex < 0 || characterIndex >= static_cast<int>(m_availableCharacters.size())) {
        return;
    }

    const CharacterData &data = m_availableCharacters[characterIndex];

    // 绘制卡片背景
    sf::RectangleShape card(sf::Vector2f(m_cardWidth, m_cardHeight));
    card.setPosition(position);

    if (isSelected) {
        if (playerIndex == 0) {
            card.setFillColor(sf::Color(m_player1Color.r, m_player1Color.g, m_player1Color.b, 100));
            card.setOutlineColor(m_player1Color);
        } else if (playerIndex == 1) {
            card.setFillColor(sf::Color(m_player2Color.r, m_player2Color.g, m_player2Color.b, 100));
            card.setOutlineColor(m_player2Color);
        } else {
            // 两个玩家都选中
            card.setFillColor(sf::Color(200, 150, 200, 100));
            card.setOutlineColor(sf::Color(255, 200, 255));
        }
        card.setOutlineThickness(3.0f);
    } else {
        card.setFillColor(sf::Color(50, 50, 50, 200));
        card.setOutlineColor(sf::Color(100, 100, 100));
        card.setOutlineThickness(1.0f);
    }

    window.draw(card);

    // 绘制角色名称
    const sf::Font &font = ResourceManager::getInstance().getFont("default");
    const sf::Font &chFont = ResourceManager::getInstance().getFont("ch");

    sf::Text nameText(chFont);
    sf::String u8Text = sf::String::fromUtf8(data.name.begin(), data.name.end());
    nameText.setString(u8Text);
    nameText.setCharacterSize(28);
    nameText.setFillColor(sf::Color::White);

    sf::FloatRect nameBounds = nameText.getLocalBounds();
    nameText.setPosition(sf::Vector2f(
        position.x + (m_cardWidth - nameBounds.size.x) / 2.0f,
        position.y + 20.0f));

    window.draw(nameText);

    // 绘制角色描述
    sf::Text descText(chFont);
    u8Text = sf::String::fromUtf8(data.description.begin(), data.description.end());
    descText.setString(data.description);
    descText.setCharacterSize(16);
    descText.setFillColor(sf::Color(180, 180, 180));

    sf::FloatRect descBounds = descText.getLocalBounds();
    descText.setPosition(sf::Vector2f(
        position.x + (m_cardWidth - descBounds.size.x) / 2.0f,
        position.y + 60.0f));

    window.draw(descText);

    // 绘制角色属性摘要
    sf::Text statsText(font);
    statsText.setString(
        "HP: " + std::to_string(static_cast<int>(data.stats.maxHp)) + "\n"
                                                                      "Speed: " +
        std::to_string(static_cast<int>(data.stats.moveSpeed)));
    statsText.setCharacterSize(18);
    statsText.setFillColor(sf::Color(200, 200, 200));
    statsText.setPosition(sf::Vector2f(position.x + 20.0f, position.y + 100.0f));

    window.draw(statsText);
}

void CharacterSelectScene::renderPlayerArea(sf::RenderWindow &window, int playerIndex) {
    const PlayerSelection &selection = (playerIndex == 0) ? m_player1Selection : m_player2Selection;
    sf::Text *label = (playerIndex == 0) ? m_player1Label.get() : m_player2Label.get();
    sf::Color playerColor = (playerIndex == 0) ? m_player1Color : m_player2Color;

    // 绘制玩家标签
    if (label) {
        window.draw(*label);
    }

    // 绘制选择状态
    float areaX = (playerIndex == 0) ? 100.0f : WINDOW_WIDTH - 300.0f;
    float areaY = 170.0f;

    const sf::Font &font = ResourceManager::getInstance().getDefaultFont();
    const sf::Font &chFont = ResourceManager::getInstance().getFont("ch");

    if (selection.characterIndex >= 0 && selection.characterIndex < static_cast<int>(m_availableCharacters.size())) {
        const CharacterData &data = m_availableCharacters[selection.characterIndex];

        // 显示选中的角色名称
        sf::Text charNameText(chFont);
        sf::String u8Text = sf::String::fromUtf8(data.name.begin(), data.name.end());
        charNameText.setString(u8Text);
        charNameText.setCharacterSize(24);
        charNameText.setFillColor(playerColor);
        charNameText.setPosition(sf::Vector2f(areaX, areaY));

        window.draw(charNameText);

        // 显示确认状态
        sf::Text statusText(font);
        if (selection.confirmed) {
            statusText.setString("READY!");
            statusText.setFillColor(m_confirmedColor);
        } else {
            statusText.setString("Selecting...");
            statusText.setFillColor(sf::Color(150, 150, 150));
        }
        statusText.setCharacterSize(20);
        statusText.setPosition(sf::Vector2f(areaX, areaY + 35.0f));

        window.draw(statusText);

        // 显示角色详细信息
        renderCharacterInfo(window, data, sf::Vector2f(areaX, areaY + 70.0f));
    }
}

void CharacterSelectScene::renderCharacterInfo(sf::RenderWindow &window, const CharacterData &data,
                                               const sf::Vector2f &position) {
    const sf::Font &font = ResourceManager::getInstance().getDefaultFont();

    // 显示角色属性
    std::string info =
        "Max HP: " + std::to_string(static_cast<int>(data.stats.maxHp)) + "\n"
                                                                          "Max Stamina: " +
        std::to_string(static_cast<int>(data.stats.maxStamina)) + "\n"
                                                                  "Move Speed: " +
        std::to_string(static_cast<int>(data.stats.moveSpeed)) + "\n"
                                                                 "Jump Height: " +
        std::to_string(static_cast<int>(data.stats.jumpHeight));

    sf::Text infoText(font);
    infoText.setString(info);
    infoText.setCharacterSize(16);
    infoText.setFillColor(sf::Color(180, 180, 180));
    infoText.setPosition(position);

    window.draw(infoText);
}

} // namespace SamuraiFight
