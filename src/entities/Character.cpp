#include "Character.h"
#include "../core/Constants.h"
#include "../managers/AudioManager.h"
#include "../managers/ResourceManager.h"
#include "../states/AttackState.h"
#include "../states/StateMachine.h"

#include <iostream>

namespace SamuraiFight {

Character::Character(const CharacterData &data, int playerIndex)
    : Entity(), m_data(data), m_stateMachine(nullptr), m_animation(nullptr), m_hitbox(nullptr), m_health(nullptr), m_stamina(nullptr), m_inputBuffer(nullptr), m_playerIndex(playerIndex), m_facingRight(true), m_frameCount(0), m_moveLeft(false), m_moveRight(false), m_runLeft(false), m_runRight(false), m_jump(false), m_attackLight(false), m_attackMedium(false), m_attackHeavy(false), m_showDebugHitboxes(false), m_pendingStunFrames(10), m_blockCooldown(false), m_blockCooldownTimer(0) {
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
    const sf::Texture &texture = ResourceManager::getInstance().getTexture(initialTexture);
    m_sprite = std::make_unique<sf::Sprite>(texture);

    // 创建动画组件
    m_animation = std::make_unique<AnimationComponent>(*m_sprite);

    // 加载动画
    for (const auto &[name, animData] : m_data.animations) {
        m_animation->loadAnimation(name, animData);
    }

    // 创建碰撞框组件
    m_hitbox = std::make_unique<HitboxComponent>();

    // 加载碰撞框数据
    for (const auto &[state, hitboxData] : m_data.hitboxes) {
        m_hitbox->loadHitboxes(state, hitboxData);
    }

    // 创建生命值组件
    m_health = std::make_unique<HealthComponent>(m_data.stats.maxHp);

    // 创建体力值组件
    m_stamina = std::make_unique<StaminaComponent>(m_data.stats.maxStamina);

    // 创建输入缓冲
    m_inputBuffer = std::make_unique<InputBuffer>(INPUT_BUFFER_SIZE);

    // 创建状态机
    m_stateMachine = std::make_unique<StateMachine>(this);
}

void Character::handleInput(bool moveLeft, bool moveRight, bool runLeft, bool runRight, bool jump,
                            bool attackLight, bool attackMedium, bool attackHeavy, bool block, bool roll) {

    // 防止同时按左和右
    if (moveLeft && moveRight) {
        moveLeft = moveRight = false;
    }
    if (runLeft && runRight) {
        runLeft = runRight = false;
    }

    m_moveLeft = moveLeft;
    m_moveRight = moveRight;
    m_runLeft = runLeft;
    m_runRight = runRight;
    m_jump = jump;
    m_attackLight = attackLight;
    m_attackMedium = attackMedium;
    m_attackHeavy = attackHeavy;

    // 获取当前状态
    CharacterStateType currentState = getCurrentStateType();

    // ========== 跳跃状态下的空中控制 ==========
    if (currentState == CharacterStateType::Jump) {
        // 更新朝向
        if (moveRight || runRight) {
            m_facingRight = true;
        } else if (moveLeft || runLeft) {
            m_facingRight = false;
        }

        // 翻滚（空中，需要体力>20%）
        if (roll && m_stamina && m_stamina->canEnterStaminaState()) {
            changeState(CharacterStateType::Roll);
            return;
        }

        // 空中攻击
        if (attackHeavy) {
            changeState(CharacterStateType::AttackHeavy);
            return;
        }
        if (attackMedium) {
            changeState(CharacterStateType::AttackMedium);
            return;
        }
        if (attackLight) {
            changeState(CharacterStateType::AttackLight);
            return;
        }

        // 空中移动控制（降低速度）
        float airMoveSpeed = m_data.stats.moveSpeed * 0.8f;
        sf::Vector2f velocity = getVelocity();

        if (moveLeft || runLeft) {
            velocity.x = -airMoveSpeed;
        } else if (moveRight || runRight) {
            velocity.x = airMoveSpeed;
        } else {
            velocity.x *= 0.95f;
        }

        setVelocity(velocity);
        return;
    }

    // ========== 地面状态处理 ==========

    // ========== 死亡状态处理 ==========
    if (currentState == CharacterStateType::Dead) {
        // 死亡状态下不接受任何输入
        return;
    }

    // ========== 受击状态处理 ==========
    if (currentState == CharacterStateType::Hurt) {
        // 受击状态下不接受任何输入
        return;
    }

    // ========== 翻滚状态处理 ==========
    if (currentState == CharacterStateType::Roll) {
        // 翻滚期间不接受任何输入，等待自动结束
        return;
    }

    // ========== 防御状态处理 ==========
    if (currentState == CharacterStateType::Block) {
        // 松开防御键或体力耗尽时退出防御
        if (!block) {
            changeState(CharacterStateType::Idle);
            return;
        }
        // 防御状态下允许移动，但不能改变朝向
        if (moveLeft || moveRight) {
            // 允许移动，但速度变慢（在update中处理）
            float blockMoveSpeed = m_data.stats.moveSpeed * 0.5f; // 防御时移动速度50%
            sf::Vector2f velocity = getVelocity();
            if (moveLeft) {
                velocity.x = -blockMoveSpeed;
            } else if (moveRight) {
                velocity.x = blockMoveSpeed;
            }
            setVelocity(velocity);
        } else {
            // 停止移动
            sf::Vector2f velocity = getVelocity();
            velocity.x = 0.0f;
            setVelocity(velocity);
        }
        // 不改变朝向，不处理其他输入
        return;
    }

    // ========== 攻击状态处理 ==========
    bool isAttackState = (currentState == CharacterStateType::AttackLight ||
                          currentState == CharacterStateType::AttackMedium ||
                          currentState == CharacterStateType::AttackHeavy);

    if (isAttackState) {
        // 检查是否可以取消当前攻击
        if (canCancelAttack()) {
            // 优先级：重 > 中 > 轻
            if (attackHeavy && currentState != CharacterStateType::AttackHeavy) {
                changeState(CharacterStateType::AttackHeavy);
                return;
            }
            if (attackMedium && currentState != CharacterStateType::AttackMedium) {
                changeState(CharacterStateType::AttackMedium);
                return;
            }
            if (attackLight && currentState != CharacterStateType::AttackLight) {
                changeState(CharacterStateType::AttackLight);
                return;
            }
        }
        // 攻击状态下不处理其他输入
        return;
    }

    // 地面攻击（站立或下蹲状态）
    if (m_onGround) {
        // 防御（优先级低于攻击）
        if (block && canBlock() && !m_blockCooldown) {
            changeState(CharacterStateType::Block);
            return;
        }

        if (attackHeavy) {
            changeState(CharacterStateType::AttackHeavy);
            return;
        }
        if (attackMedium) {
            changeState(CharacterStateType::AttackMedium);
            return;
        }
        if (attackLight) {
            changeState(CharacterStateType::AttackLight);
            return;
        }
    }

    // 翻滚（空中地面均可，需要体力>20%）
    if (roll && m_stamina && m_stamina->canEnterStaminaState()) {
        changeState(CharacterStateType::Roll);
        return;
    }

    // 跳跃优先级最高（需要体力大于20%阈值）
    if (jump && m_onGround && currentState != CharacterStateType::Jump) {
        if (m_stamina && m_stamina->canEnterStaminaState()) {
            changeState(CharacterStateType::Jump);
            return;
        }
    }

    // 跑步（需要体力大于20%阈值）
    if ((runLeft || runRight) && m_onGround &&
        currentState != CharacterStateType::Run) {
        if (m_stamina && m_stamina->canEnterStaminaState()) {
            if (runRight) {
                m_facingRight = true;
            } else if (runLeft) {
                m_facingRight = false;
            }
            changeState(CharacterStateType::Run);
            return;
        }
    }

    // 走路（不需要体力阈值限制）
    if ((moveLeft || moveRight) && m_onGround &&
        currentState != CharacterStateType::Walk) {
        if (moveRight) {
            m_facingRight = true;
        } else if (moveLeft) {
            m_facingRight = false;
        }
        changeState(CharacterStateType::Walk);
        return;
    }

    // 如果在跑步状态，停止跑步后回到站立
    if (currentState == CharacterStateType::Run && !runLeft && !runRight) {
        changeState(CharacterStateType::Idle);
        return;
    }

    // 如果在走路状态，停止走路后回到站立
    if (currentState == CharacterStateType::Walk && !moveLeft && !moveRight) {
        changeState(CharacterStateType::Idle);
        return;
    }

    // 更新朝向（在地面跑步中）
    if (currentState == CharacterStateType::Run) {
        if (runRight) {
            m_facingRight = true;
        } else if (runLeft) {
            m_facingRight = false;
        }
    }

    // 更新朝向（在地面走路中）
    if (currentState == CharacterStateType::Walk) {
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

    // 更新防御冷却
    if (m_blockCooldown) {
        m_blockCooldownTimer++;
        // 冷却时间：60帧（1秒）
        if (m_blockCooldownTimer >= 60) {
            m_blockCooldown = false;
            m_blockCooldownTimer = 0;
        }
    }

    // 更新输入缓冲
    updateInputBuffer();

    // 应用重力
    applyGravity(deltaTime);

    // 防御冷却期间移动速度变慢（防御状态的速度已在handleInput中处理）
    if (getCurrentStateType() != CharacterStateType::Block && m_blockCooldown) {
        // 冷却期间移动速度为正常的70%
        m_velocity.x *= 0.7f;
    }

    // 更新位置
    m_position += m_velocity * deltaTime;

    // 检测地面碰撞（在状态机更新之前）
    checkGroundCollision();

    // 检测屏幕边界
    checkScreenBounds();

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
        int frame = getCurrentAnimationFrame();

        switch (getCurrentStateType()) {
        case CharacterStateType::Idle:
            stateName = "idle";
            break;
        case CharacterStateType::Walk:
            stateName = "walk";
            break;
        case CharacterStateType::Run:
            stateName = "run";
            break;
        case CharacterStateType::Jump:
            stateName = "jump";
            break;
        case CharacterStateType::AttackLight:
        case CharacterStateType::AttackMedium:
        case CharacterStateType::AttackHeavy: {
            // 攻击状态使用攻击帧数
            AttackState *attackState = dynamic_cast<AttackState *>(m_stateMachine->getCurrentState());
            if (attackState) {
                frame = attackState->getCurrentAttackFrame();
            }
            if (getCurrentStateType() == CharacterStateType::AttackLight) {
                stateName = "attack_light";
            } else if (getCurrentStateType() == CharacterStateType::AttackMedium) {
                stateName = "attack_medium";
            } else {
                stateName = "attack_heavy";
            }
        } break;
        case CharacterStateType::Hurt:
            stateName = "hurt";
            break;
        case CharacterStateType::Block:
            stateName = "block";
            break;
        case CharacterStateType::Roll:
            stateName = "roll";
            break;
        default:
            stateName = "idle";
            break;
        }
        m_hitbox->update(m_position, stateName, frame, m_facingRight);
    }

    // 更新体力值
    if (m_stamina) {
        bool isWalking = (getCurrentStateType() == CharacterStateType::Walk);
        bool isRunning = (getCurrentStateType() == CharacterStateType::Run);
        bool isJumping = (getCurrentStateType() == CharacterStateType::Jump);
        bool isStanding = (getCurrentStateType() == CharacterStateType::Idle && m_onGround);

        m_stamina->update(
            isWalking || isRunning,
            isJumping,
            isStanding,
            m_data.stats.moveStaminaRecovery,
            m_data.stats.staminaRecovery,
            m_data.stats.jumpStaminaCost);
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
    if (!m_inputBuffer)
        return;

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

void Character::render(sf::RenderWindow &window) {
    if (m_sprite) {
        window.draw(*m_sprite);
    }

    // 渲染调试碰撞框
    if (m_showDebugHitboxes && m_hitbox) {
        m_hitbox->renderDebug(window);
    }
}

void Character::changeState(CharacterStateType stateType) {
    if (m_stateMachine) {
        m_stateMachine->changeState(stateType);
    }
}

void Character::playAnimation(const std::string &name) {
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

const CharacterData &Character::getData() const {
    return m_data;
}

InputBuffer &Character::getInputBuffer() {
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
        if (!m_onGround) {
            AudioManager::getInstance().playCharacterSound(getData().id, "land");
        }
        m_position.y = GROUND_LEVEL;
        m_velocity.y = 0.0f;
        m_onGround = true;
    } else {
        m_onGround = false;
    }
}

void Character::checkScreenBounds() {
    // 获取当前状态的碰撞框
    std::string stateName;
    switch (getCurrentStateType()) {
    case CharacterStateType::Idle:
        stateName = "idle";
        break;
    case CharacterStateType::Walk:
        stateName = "walk";
        break;
    case CharacterStateType::Run:
        stateName = "run";
        break;
    case CharacterStateType::Jump:
        stateName = "jump";
        break;
    case CharacterStateType::AttackLight:
        stateName = "attack_light";
        break;
    case CharacterStateType::AttackMedium:
        stateName = "attack_medium";
        break;
    case CharacterStateType::AttackHeavy:
        stateName = "attack_heavy";
        break;
    case CharacterStateType::Hurt:
        stateName = "hurt";
        break;
    default:
        stateName = "idle";
        break;
    }

    // 临时更新碰撞框位置以获取边界
    if (m_hitbox) {
        m_hitbox->update(m_position, stateName, getCurrentAnimationFrame(), m_facingRight);

        // 获取所有受击框来确定角色边界
        auto hurtboxes = m_hitbox->getHurtboxes();
        if (!hurtboxes.empty()) {
            // 计算角色的实际边界
            float minX = m_position.x;
            float maxX = m_position.x;

            for (const auto &hurtbox : hurtboxes) {
                float left = hurtbox.rect.position.x;
                float right = left + hurtbox.rect.size.x;
                minX = std::min(minX, left);
                maxX = std::max(maxX, right);
            }

            // 检查左边界
            if (minX < 0.0f) {
                m_position.x -= minX; // 修正到边界内
            }

            // 检查右边界
            if (maxX > WINDOW_WIDTH) {
                m_position.x -= (maxX - WINDOW_WIDTH); // 修正到边界内
            }
        }
    }
}

HitboxComponent *Character::getHitboxComponent() {
    return m_hitbox.get();
}

const HitboxComponent *Character::getHitboxComponent() const {
    return m_hitbox.get();
}

StateMachine *Character::getStateMachine() {
    return m_stateMachine.get();
}

bool Character::canCancelAttack() const {
    if (!m_stateMachine)
        return false;

    CharacterState *state = m_stateMachine->getCurrentState();
    if (!state)
        return false;

    CharacterStateType type = state->getType();
    if (type == CharacterStateType::AttackLight ||
        type == CharacterStateType::AttackMedium ||
        type == CharacterStateType::AttackHeavy) {
        // 从AttackState获取取消信息
        AttackState *attackState = dynamic_cast<AttackState *>(state);
        if (attackState) {
            return attackState->canCancel();
        }
    }
    return false;
}

bool Character::isInAttackActiveFrames() const {
    if (!m_stateMachine)
        return false;

    CharacterStateType type = getCurrentStateType();
    if (type != CharacterStateType::AttackLight &&
        type != CharacterStateType::AttackMedium &&
        type != CharacterStateType::AttackHeavy) {
        return false;
    }

    // 需要从AttackState获取判定帧信息
    // 这里简化处理，实际应该dynamic_cast到AttackState
    return false; // 暂时返回false，后续优化
}

void Character::takeDamage(float damage, int stunFrames) {
    if (!m_health)
        return;

    // 造成伤害
    float actualDamage = m_health->takeDamage(damage);

    if (actualDamage > 0.0f) {
        std::cout << "Character " << m_data.name << " takes " << actualDamage << " damage"
                  << " (HP: " << m_health->getCurrentHp() << "/" << m_health->getMaxHp() << ")"
                  << ", stun for " << stunFrames << " frames" << std::endl;

        // 如果死亡，进入死亡状态
        if (m_health->isDead()) {
            std::cout << "Character " << m_data.name << " is dead!" << std::endl;
            // 进入死亡状态
            changeState(CharacterStateType::Dead);
        } else {
            // 进入受击硬直状态
            triggerHurt(stunFrames);
        }
    }
}

HealthComponent *Character::getHealthComponent() {
    return m_health.get();
}

const HealthComponent *Character::getHealthComponent() const {
    return m_health.get();
}

bool Character::isDead() const {
    return m_health ? m_health->isDead() : false;
}

StaminaComponent *Character::getStaminaComponent() {
    return m_stamina.get();
}

const StaminaComponent *Character::getStaminaComponent() const {
    return m_stamina.get();
}

bool Character::isExhausted() const {
    return m_stamina ? m_stamina->isExhausted() : false;
}

void Character::triggerHurt(int stunFrames) {
    m_pendingStunFrames = stunFrames;
    changeState(CharacterStateType::Hurt);
}

int Character::getPendingStunFrames() const {
    return m_pendingStunFrames;
}

void Character::setBlockCooldown(bool cooldown) {
    m_blockCooldown = cooldown;
    if (cooldown) {
        m_blockCooldownTimer = 0;
    }
}

bool Character::isInBlockCooldown() const {
    return m_blockCooldown;
}

bool Character::canBlock() const {
    // 需要体力值大于20%阈值才能进入防御状态
    if (m_stamina) {
        return m_stamina->canEnterStaminaState();
    }
    return false;
}

void Character::setShowDebugHitboxes(bool show) {
    m_showDebugHitboxes = show;
}

bool Character::isShowDebugHitboxes() const {
    return m_showDebugHitboxes;
}

} // namespace SamuraiFight