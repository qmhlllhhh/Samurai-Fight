#include "Character.h"
#include "../core/Constants.h"
#include "../managers/ResourceManager.h"
#include "../states/StateMachine.h"

#include <iostream>

namespace SamuraiFight {

Character::Character(const CharacterData& data, int playerIndex)
    : Entity()
    , m_data(data)
    , m_stateMachine(nullptr)
    , m_animation(nullptr)
    , m_hitbox(nullptr)
    , m_inputBuffer(nullptr)
    , m_playerIndex(playerIndex)
    , m_facingRight(true)
    , m_frameCount(0)
    , m_moveLeft(false)
    , m_moveRight(false)
    , m_jump(false)
    , m_crouch(false)
    , m_attackLight(false)
    , m_attackMedium(false)
    , m_attackHeavy(false) {
    // 设置初始位置
    float startX = (playerIndex == 0) ? 300.0f : WINDOW_WIDTH - 300.0f;
    setPosition(sf::Vector2f(startX, GROUND_LEVEL));

    // 设置朝向
    m_facingRight = (playerIndex == 0);

    // 初始化组件
    initializeComponents();

    std::cout << "Character: Created " << m_data.name << " (Player " << playerIndex + 1 << ")" << std::endl;
}

Character::~Character() {
}

void Character::initializeComponents() {
    // 创建精灵（需要纹理）
    std::string initialTexture = "assets/characters/musashi/idle.png";
    if (!m_data.animations.empty()) {
        initialTexture = m_data.animations.begin()->second.spriteSheet;
    }

    // 加载初始纹理
    if (!ResourceManager::getInstance().hasTexture(initialTexture)) {
        ResourceManager::getInstance().loadTexture(initialTexture, initialTexture);
    }

    // 创建精灵
    const sf::Texture& texture = ResourceManager::getInstance().getTexture(initialTexture);
    m_sprite = std::make_unique<sf::Sprite>(texture);

    // 创建动画组件
    m_animation = std::make_unique<AnimationComponent>(*m_sprite);

    // 加载动画
    for (const auto& [name, animData] : m_data.animations) {
        m_animation->loadAnimation(name, animData);
    }

    // 创建碰撞框组件
    m_hitbox = std::make_unique<HitboxComponent>();

    // 加载碰撞框数据
    for (const auto& [state, hitboxData] : m_data.hitboxes) {
        m_hitbox->loadHitboxes(state, hitboxData);
    }

    // 创建输入缓冲
    m_inputBuffer = std::make_unique<InputBuffer>(INPUT_BUFFER_SIZE);

    // 创建状态机
    m_stateMachine = std::make_unique<StateMachine>(this);
}

void Character::handleInput(bool moveLeft, bool moveRight, bool jump, bool crouch,
                            bool attackLight, bool attackMedium, bool attackHeavy) {
    m_moveLeft = moveLeft;
    m_moveRight = moveRight;
    m_jump = jump;
    m_crouch = crouch;
    m_attackLight = attackLight;
    m_attackMedium = attackMedium;
    m_attackHeavy = attackHeavy;

    // 获取当前状态
    CharacterStateType currentState = getCurrentStateType();

    // ========== 跳跃状态下的空中控制 ==========
    if (currentState == CharacterStateType::Jump) {
        // 更新朝向
        if (moveRight) {
            m_facingRight = true;
        } else if (moveLeft) {
            m_facingRight = false;
        }

        // 空中移动控制（降低速度）
        float airMoveSpeed = m_data.stats.moveSpeed * 0.8f;
        sf::Vector2f velocity = getVelocity();

        if (moveLeft) {
            velocity.x = -airMoveSpeed;
        } else if (moveRight) {
            velocity.x = airMoveSpeed;
        } else {
            velocity.x *= 0.95f;
        }

        setVelocity(velocity);
        return;
    }

    // ========== 地面状态处理 ==========

    // 跳跃优先级最高
    if (jump && m_onGround && currentState != CharacterStateType::Jump) {
        changeState(CharacterStateType::Jump);
        return;
    }

    // 下蹲
    if (crouch && m_onGround && currentState != CharacterStateType::Crouch) {
        changeState(CharacterStateType::Crouch);
        return;
    }

    // 如果在下蹲状态，松开下蹲键后回到站立
    if (currentState == CharacterStateType::Crouch && !crouch) {
        changeState(CharacterStateType::Idle);
        return;
    }

    // 移动
    if ((moveLeft || moveRight) && m_onGround &&
        currentState != CharacterStateType::Move &&
        currentState != CharacterStateType::Crouch) {
        if (moveRight) {
            m_facingRight = true;
        } else if (moveLeft) {
            m_facingRight = false;
        }
        changeState(CharacterStateType::Move);
        return;
    }

    // 如果在移动状态，停止移动后回到站立
    if (currentState == CharacterStateType::Move && !moveLeft && !moveRight) {
        changeState(CharacterStateType::Idle);
        return;
    }

    // 更新朝向（在地面移动中）
    if (currentState == CharacterStateType::Move) {
        if (moveRight) {
            m_facingRight = true;
        } else if (moveLeft) {
            m_facingRight = false;
        }
    }
}

void Character::update(float deltaTime) {
    // 更新帧计数
    m_frameCount++;

    // 更新输入缓冲
    updateInputBuffer();

    // 应用重力
    applyGravity(deltaTime);

    // 更新位置
    m_position += m_velocity * deltaTime;

    // 检测地面碰撞（在状态机更新之前）
    checkGroundCollision();

    // 更新状态机
    if (m_stateMachine) {
        m_stateMachine->update(deltaTime);
    }

    // 更新动画
    if (m_animation) {
        m_animation->update(deltaTime);
    }

    // 更新碰撞框
    if (m_hitbox) {
        std::string stateName;
        switch (getCurrentStateType()) {
        case CharacterStateType::Idle:
            stateName = "idle";
            break;
        case CharacterStateType::Move:
            stateName = "walk";
            break;
        case CharacterStateType::Jump:
            stateName = "jump";
            break;
        case CharacterStateType::Crouch:
            stateName = "crouch";
            break;
        default:
            stateName = "idle";
            break;
        }
        m_hitbox->update(m_position, stateName, getCurrentAnimationFrame(), m_facingRight);
    }

    // 更新精灵位置
    if (m_sprite) {
        m_sprite->setPosition(m_position);

        // 根据朝向翻转精灵
        sf::Vector2f scale = m_sprite->getScale();
        if (m_facingRight && scale.x < 0) {
            m_sprite->setScale(sf::Vector2f(-scale.x, scale.y));
        } else if (!m_facingRight && scale.x > 0) {
            m_sprite->setScale(sf::Vector2f(-scale.x, scale.y));
        }
    }
}

void Character::updateInputBuffer() {
    if (!m_inputBuffer) return;

    m_inputBuffer->setCurrentFrame(m_frameCount);

    // 记录攻击输入（按下事件）
    // 注意：这里需要检测"按下瞬间"，而不是持续按住
    // 简化实现：每帧记录当前按键状态

    if (m_attackLight) {
        m_inputBuffer->pushInput(InputAction::AttackLight, true, m_frameCount);
    }
    if (m_attackMedium) {
        m_inputBuffer->pushInput(InputAction::AttackMedium, true, m_frameCount);
    }
    if (m_attackHeavy) {
        m_inputBuffer->pushInput(InputAction::AttackHeavy, true, m_frameCount);
    }
}

void Character::render(sf::RenderWindow& window) {
    if (m_sprite) {
        window.draw(*m_sprite);
    }

    // 调试模式：渲染碰撞框
    #ifdef DEBUG
    if (m_hitbox) {
        m_hitbox->renderDebug(window);
    }
    #endif
}

void Character::changeState(CharacterStateType stateType) {
    if (m_stateMachine) {
        m_stateMachine->changeState(stateType);
    }
}

void Character::playAnimation(const std::string& name) {
    if (m_animation) {
        m_animation->play(name);
    }
}

float Character::getMoveSpeed() const {
    return m_data.stats.moveSpeed;
}

float Character::getJumpHeight() const {
    return m_data.stats.jumpHeight;
}

CharacterStateType Character::getCurrentStateType() const {
    if (m_stateMachine) {
        return m_stateMachine->getCurrentStateType();
    }
    return CharacterStateType::Idle;
}

int Character::getCurrentAnimationFrame() const {
    if (m_animation) {
        return m_animation->getCurrentFrame();
    }
    return 0;
}

bool Character::isFacingRight() const {
    return m_facingRight;
}

void Character::setFacingRight(bool facingRight) {
    m_facingRight = facingRight;
}

int Character::getPlayerIndex() const {
    return m_playerIndex;
}

const CharacterData& Character::getData() const {
    return m_data;
}

InputBuffer& Character::getInputBuffer() {
    return *m_inputBuffer;
}

int Character::getFrameCount() const {
    return m_frameCount;
}

void Character::applyGravity(float deltaTime) {
    // 只有在空中才应用重力
    if (!m_onGround) {
        m_velocity.y += GRAVITY * deltaTime;
    }
}

void Character::checkGroundCollision() {
    // 检测是否落地
    if (m_position.y >= GROUND_LEVEL) {
        m_position.y = GROUND_LEVEL;
        m_velocity.y = 0.0f;
        m_onGround = true;
    } else {
        m_onGround = false;
    }
}

} // namespace SamuraiFight