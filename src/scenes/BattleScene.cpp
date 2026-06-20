#include "BattleScene.h"
#include "../core/Constants.h"
#include "../entities/CharacterFactory.h"
#include "../managers/AudioManager.h"
#include "../managers/ResourceManager.h"
#include "../states/AttackState.h"
#include "../states/BlockState.h"
#include "../states/StateMachine.h"
#include "PauseScene.h"
#include <iostream>

namespace SamuraiFight {

BattleScene::BattleScene()
    : m_backgroundId{"grass3", "house", "grass1"}, m_window(nullptr), m_player1CharacterId("musashi") // 默认角色
      ,
      m_player2CharacterId("sakura") // 默认角色
      ,
      m_inputManager(nullptr), m_showDebug(false) {
}

BattleScene::BattleScene(const std::string &player1CharacterId, const std::string &player2CharacterId)
    : m_backgroundId{"grass3", "house", "grass1"}, m_window(nullptr), m_player1CharacterId(player1CharacterId), m_player2CharacterId(player2CharacterId), m_inputManager(nullptr), m_showDebug(false) {
}

BattleScene::~BattleScene() {
}

void BattleScene::onEnter() {
    std::cout << "BattleScene: Entered" << std::endl;

    // 结束正在播放的音乐
    AudioManager::getInstance().stopMusic(1.0f);
    // 播放背景音乐
    AudioManager::getInstance().playSceneMusic("battle", 2.0f);

    // 初始化输入管理器
    m_inputManager = std::make_unique<InputManager>();
    m_inputManager->initialize();

    // 初始化角色
    initializeCharacters();

    // 初始化背景
    initializeBackground();

    // 初始化调试文本
    try {
        const sf::Font &font = ResourceManager::getInstance().getDefaultFont();
        m_debugText = std::make_unique<sf::Text>(font);
        m_debugText->setCharacterSize(16);
        m_debugText->setFillColor(sf::Color::White);
        m_debugText->setPosition(sf::Vector2f(10.0f, 10.0f));
    } catch (const std::exception &e) {
        std::cerr << "BattleScene: Error initializing debug text - " << e.what() << std::endl;
    }

    std::cout << "BattleScene: Initialization complete" << std::endl;
}

void BattleScene::onExit() {
    std::cout << "BattleScene: Exited" << std::endl;

    // 清理角色
    m_characters[0].reset();
    m_characters[1].reset();

    // 清理输入管理器
    m_inputManager.reset();
}

void BattleScene::initializeCharacters() {
    // 加载角色数据
    CharacterFactory &factory = CharacterFactory::getInstance();

    // 创建玩家1角色
    m_characters[0] = factory.createCharacter(m_player1CharacterId, 0);
    if (m_characters[0]) {
        std::cout << "BattleScene: Created player 1 character '" << m_player1CharacterId << "'" << std::endl;
    } else {
        std::cerr << "BattleScene: Failed to create player 1 character '" << m_player1CharacterId << "'" << std::endl;
    }

    // 创建玩家2角色
    m_characters[1] = factory.createCharacter(m_player2CharacterId, 1);
    if (m_characters[1]) {
        std::cout << "BattleScene: Created player 2 character '" << m_player2CharacterId << "'" << std::endl;
    } else {
        std::cerr << "BattleScene: Failed to create player 2 character '" << m_player2CharacterId << "'" << std::endl;
    }

    // 初始化血条UI
    // 玩家1血条在左上角，玩家2血条在右上角
    float barWidth = 300.0f;
    float barHeight = 25.0f;
    float staminaBarHeight = 12.0f;
    float barY = 30.0f;
    float staminaBarY = barY + barHeight + 5.0f; // 体力条在血条下方
    float margin = 50.0f;

    m_healthBars[0] = std::make_unique<HealthBar>(
        sf::Vector2f(margin, barY),
        sf::Vector2f(barWidth, barHeight),
        true // 玩家1
    );

    m_healthBars[1] = std::make_unique<HealthBar>(
        sf::Vector2f(WINDOW_WIDTH - margin - barWidth, barY),
        sf::Vector2f(barWidth, barHeight),
        false // 玩家2
    );

    // 初始化体力条UI
    m_staminaBars[0] = std::make_unique<StaminaBar>(
        sf::Vector2f(margin, staminaBarY),
        sf::Vector2f(barWidth, staminaBarHeight),
        true // 玩家1
    );

    m_staminaBars[1] = std::make_unique<StaminaBar>(
        sf::Vector2f(WINDOW_WIDTH - margin - barWidth, staminaBarY),
        sf::Vector2f(barWidth, staminaBarHeight),
        false // 玩家2
    );
}

void BattleScene::initializeBackground() {
    // 随机选择背景
    int bgIndex = randomInt(0, BACKGROUND_COUNT - 1);
    std::string bgId = m_backgroundId[bgIndex];

    // 检查纹理是否存在
    if (ResourceManager::getInstance().hasTexture(bgId)) {
        const sf::Texture &texture = ResourceManager::getInstance().getTexture(bgId);
        m_background = std::make_unique<sf::Sprite>(texture);
        m_background->setPosition({0, 0});
        m_background->setScale({static_cast<float>(WINDOW_WIDTH) / texture.getSize().x,
                                static_cast<float>(WINDOW_HEIGHT) / texture.getSize().y});
    } else {
        std::cerr << "BattleScene: Background texture '" << bgId << "' not found" << std::endl;
        m_background = nullptr;
    }
}

void BattleScene::handleEvents(sf::RenderWindow &window) {
    // 保存窗口指针
    m_window = &window;

    // 处理窗口事件
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        // 窗口关闭
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        // 键盘按键
        if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyPressed->code) {
            case sf::Keyboard::Key::Escape:
                // 暂停游戏（压入暂停场景）
                m_popSceneCount = 0;
                m_nextScene = std::make_unique<PauseScene>();
                break;

            case sf::Keyboard::Key::F1:
                // 切换调试信息显示
                m_showDebug = !m_showDebug;
                // 同时切换角色的调试碰撞框显示
                for (int i = 0; i < 2; ++i) {
                    if (m_characters[i]) {
                        m_characters[i]->setShowDebugHitboxes(m_showDebug);
                    }
                }
                break;

            default:
                break;
            }
        }
    }
}

void BattleScene::update(float deltaTime) {
    // 更新输入管理器
    if (m_inputManager) {
        m_inputManager->update(*m_window);

        // 获取玩家输入并传递给角色
        for (int i = 0; i < 2; ++i) {
            if (m_characters[i]) {
                InputState input = m_inputManager->getPlayerInput(i);
                m_characters[i]->handleInput(
                    input.moveLeft,
                    input.moveRight,
                    input.jump,
                    input.crouch,
                    input.attackLight,
                    input.attackMedium,
                    input.attackHeavy,
                    input.block,
                    input.roll);
            }
        }
    }

    // 更新角色
    for (int i = 0; i < 2; ++i) {
        if (m_characters[i]) {
            m_characters[i]->update(deltaTime);
        }
    }

    // 检查碰撞
    checkCollisions();

    // 更新血条UI
    for (int i = 0; i < 2; ++i) {
        if (m_characters[i] && m_healthBars[i]) {
            const HealthComponent *health = m_characters[i]->getHealthComponent();
            if (health) {
                m_healthBars[i]->update(health->getCurrentHp(), health->getMaxHp());
            }
        }
    }

    // 更新体力条UI
    for (int i = 0; i < 2; ++i) {
        if (m_characters[i] && m_staminaBars[i]) {
            const StaminaComponent *stamina = m_characters[i]->getStaminaComponent();
            if (stamina) {
                m_staminaBars[i]->update(
                    stamina->getCurrentStamina(),
                    stamina->getMaxStamina());
            }
        }
    }

    // 更新调试文本
    if (m_showDebug && m_characters[0]) {
        InputState input = m_inputManager->getPlayerInput(0);

        std::string debugStr = "Player 1:\n";
        debugStr += "  Position: (" + std::to_string(static_cast<int>(m_characters[0]->getPosition().x)) +
                    ", " + std::to_string(static_cast<int>(m_characters[0]->getPosition().y)) + ")\n";
        debugStr += "  Velocity: (" + std::to_string(static_cast<int>(m_characters[0]->getVelocity().x)) +
                    ", " + std::to_string(static_cast<int>(m_characters[0]->getVelocity().y)) + ")\n";
        debugStr += "  State: " + std::to_string(static_cast<int>(m_characters[0]->getCurrentStateType())) + "\n";
        debugStr += "  OnGround: " + std::string(m_characters[0]->isOnGround() ? "Yes" : "No") + "\n";
        debugStr += "  Input: L=" + std::string(input.moveLeft ? "1" : "0") +
                    " R=" + std::string(input.moveRight ? "1" : "0") +
                    " J=" + std::string(input.jump ? "1" : "0") + "\n";
        debugStr += "\nControls:\n";
        debugStr += "  WASD - Move/Jump/Crouch\n";
        debugStr += "  F1 - Toggle debug info\n";
        debugStr += "  ESC - Pause";

        m_debugText->setString(debugStr);
    }
}

void BattleScene::render(sf::RenderWindow &window) {
    // 绘制背景
    if (m_background) {
        window.draw(*m_background);
    } else {
        // 如果没有背景，使用默认颜色
        window.clear(sf::Color(50, 50, 80));
    }

    // 绘制角色
    for (int i = 0; i < 2; ++i) {
        if (m_characters[i]) {
            m_characters[i]->render(window);
        }
    }

    // 绘制UI（血条）
    for (int i = 0; i < 2; ++i) {
        if (m_healthBars[i]) {
            m_healthBars[i]->render(window);
        }
    }

    // 绘制UI（体力条）
    for (int i = 0; i < 2; ++i) {
        if (m_staminaBars[i]) {
            m_staminaBars[i]->render(window);
        }
    }

    // 绘制调试信息
    if (m_showDebug && m_debugText) {

        window.draw(*m_debugText);

        // 绘制地面
        sf::RectangleShape ground(sf::Vector2f(WINDOW_WIDTH, 5.0f));
        ground.setPosition(sf::Vector2f(0.0f, GROUND_LEVEL));
        ground.setFillColor(sf::Color(100, 100, 100));
        window.draw(ground);
    }
}

void BattleScene::checkCollisions() {
    // 检查两个角色之间的攻击碰撞
    if (m_characters[0] && m_characters[1]) {
        checkAttackCollision(0, 1);
        checkAttackCollision(1, 0);
    }
}

void BattleScene::checkAttackCollision(int attacker, int defender) {
    Character *attackerChar = m_characters[attacker].get();
    Character *defenderChar = m_characters[defender].get();

    if (!attackerChar || !defenderChar)
        return;

    // 获取攻击者的状态机
    StateMachine *stateMachine = attackerChar->getStateMachine();
    if (!stateMachine)
        return;

    // 检查是否在攻击状态
    CharacterStateType stateType = attackerChar->getCurrentStateType();
    if (stateType != CharacterStateType::AttackLight &&
        stateType != CharacterStateType::AttackMedium &&
        stateType != CharacterStateType::AttackHeavy) {
        return;
    }

    // 获取攻击状态
    AttackState *attackState = dynamic_cast<AttackState *>(stateMachine->getCurrentState());
    if (!attackState)
        return;

    // 检查是否还能命中（段数限制）
    if (!attackState->canHit())
        return;

    // 获取攻击者的碰撞框组件
    HitboxComponent *attackerHitbox = attackerChar->getHitboxComponent();
    if (!attackerHitbox)
        return;

    // 获取攻击者的攻击框
    std::vector<Hitbox> attackBoxes = attackerHitbox->getHitboxes();
    if (attackBoxes.empty())
        return;

    // 获取防御者的受击框组件
    HitboxComponent *defenderHitbox = defenderChar->getHitboxComponent();
    if (!defenderHitbox)
        return;

    // 获取防御者的受击框
    std::vector<Hitbox> hurtboxes = defenderHitbox->getHurtboxes();
    if (hurtboxes.empty())
        return;

    // 检查每个攻击框是否与受击框相交
    for (const auto &attackBox : attackBoxes) {
        // 检查攻击框是否激活（通过activeFrames）
        if (!attackBox.isActive())
            continue;

        for (const auto &hurtbox : hurtboxes) {
            // 检查是否相交
            if (attackBox.rect.findIntersection(hurtbox.rect)) {
                // 检查防御者是否在翻滚状态（翻滚期间无敌）
                if (defenderChar->getCurrentStateType() == CharacterStateType::Roll) {
                    // 翻滚期间无敌，不受到伤害
                    attackState->incrementHitCount();
                    std::cout << "BattleScene: Player " << defender + 1 << " is rolling, invincible!" << std::endl;
                    return;
                }

                // 检查防御者是否在防御状态
                bool defenderIsBlocking = (defenderChar->getCurrentStateType() == CharacterStateType::Block);

                if (defenderIsBlocking) {
                    // 防御成功！攻击者陷入硬直
                    const CharacterData &attackerData = attackerChar->getData();
                    std::string attackType;
                    if (stateType == CharacterStateType::AttackLight) {
                        attackType = "light";
                    } else if (stateType == CharacterStateType::AttackMedium) {
                        attackType = "medium";
                    } else if (stateType == CharacterStateType::AttackHeavy) {
                        attackType = "heavy";
                    }

                    auto it = attackerData.attacks.find(attackType);
                    int attackerStunFrames = 20; // 默认硬直帧
                    if (it != attackerData.attacks.end()) {
                        attackerStunFrames = it->second.stunFrames + 30; // 防御反击硬直更长
                    }

                    // 攻击者陷入硬直
                    attackerChar->triggerHurt(attackerStunFrames);

                    // 防御者解除防御（无冷却）
                    BlockState *blockState = dynamic_cast<BlockState *>(defenderChar->getStateMachine()->getCurrentState());
                    if (blockState) {
                        blockState->onBlockHit(); // 标记为被打破，不会设置冷却
                    }

                    // 增加命中次数
                    attackState->incrementHitCount();

                    std::cout << "BattleScene: Player " << defender + 1 << " blocked attack from Player " << attacker + 1
                              << "! Attacker stunned for " << attackerStunFrames << " frames." << std::endl;
                } else {
                    // 正常命中
                    const CharacterData &data = attackerChar->getData();

                    std::string attackType;
                    if (stateType == CharacterStateType::AttackLight) {
                        attackType = "light";
                    } else if (stateType == CharacterStateType::AttackMedium) {
                        attackType = "medium";
                    } else if (stateType == CharacterStateType::AttackHeavy) {
                        attackType = "heavy";
                    }

                    auto it = data.attacks.find(attackType);
                    if (it != data.attacks.end()) {
                        float damage = it->second.damage;
                        int stunFrames = it->second.stunFrames;

                        // 对防御者造成伤害
                        defenderChar->takeDamage(damage, stunFrames);

                        // 播放命中音效
                        AudioManager::getInstance().playSound("hit");

                        // 增加命中次数
                        attackState->incrementHitCount();

                        std::cout << "BattleScene: Player " << attacker + 1 << " hits Player " << defender + 1
                                  << " for " << damage << " damage! (Hit " << attackState->getCurrentHitCount()
                                  << "/" << attackState->getMaxHitCount() << ")" << std::endl;
                    }
                }

                // 一次判定只造成一次伤害，退出循环
                return;
            }
        }
    }
}

} // namespace SamuraiFight