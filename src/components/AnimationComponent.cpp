#include "AnimationComponent.h"
#include "../managers/ResourceManager.h"
#include <iostream>

namespace SamuraiFight {

AnimationComponent::AnimationComponent(sf::Sprite &sprite)
    : m_sprite(sprite), m_currentFrame(0), m_frameTimer(0.0f), m_playing(false) {
}

AnimationComponent::~AnimationComponent() {
}

bool AnimationComponent::loadAnimation(const std::string &name, const AnimationData &data) {
    // 检查纹理是否已加载
    if (!ResourceManager::getInstance().hasTexture(data.spriteSheet)) {
        // 加载纹理
        if (!ResourceManager::getInstance().loadTexture(data.spriteSheet, data.spriteSheet, 1)) {
            std::cerr << "AnimationComponent: Failed to load texture " << data.spriteSheet << std::endl;
            return false;
        }
    }

    // 存储动画数据
    m_animations[name] = data;
    return true;
}

void AnimationComponent::play(const std::string &name) {
    // 如果已经在播放这个动画，不重新开始
    if (m_currentAnimation == name && m_playing) {
        return;
    }

    // 检查动画是否存在
    if (m_animations.find(name) == m_animations.end()) {
        std::cerr << "AnimationComponent: Animation " << name << " not found" << std::endl;
        return;
    }

    // 切换动画
    m_currentAnimation = name;
    m_currentFrame = 0;
    m_frameTimer = 0.0f;
    m_playing = true;

    // 设置纹理
    const AnimationData &anim = m_animations[name];
    if (ResourceManager::getInstance().hasTexture(anim.spriteSheet)) {
        m_sprite.setTexture(ResourceManager::getInstance().getTexture(anim.spriteSheet));
    }

    // 更新精灵区域
    updateSpriteRect();
}

void AnimationComponent::update(float deltaTime) {
    if (!m_playing || m_currentAnimation.empty()) {
        return;
    }

    // 检查动画是否存在
    if (m_animations.find(m_currentAnimation) == m_animations.end()) {
        return;
    }

    const AnimationData &anim = m_animations[m_currentAnimation];

    // 更新帧计时器
    m_frameTimer += deltaTime;

    // 检查是否需要切换帧
    if (m_frameTimer >= anim.frameDuration) {
        m_frameTimer -= anim.frameDuration;
        m_currentFrame++;

        // 检查是否到达最后一帧
        if (m_currentFrame >= anim.frameCount) {
            if (anim.loop) {
                m_currentFrame = 0;
            } else {
                m_currentFrame = anim.frameCount - 1;
                m_playing = false;
            }
        }

        // 更新精灵区域
        updateSpriteRect();
    }
}

bool AnimationComponent::isFinished() const {
    if (m_currentAnimation.empty()) {
        return true;
    }

    auto it = m_animations.find(m_currentAnimation);
    if (it == m_animations.end()) {
        return true;
    }

    const AnimationData &anim = it->second;
    return !anim.loop && m_currentFrame >= anim.frameCount - 1;
}

std::string AnimationComponent::getCurrentAnimation() const {
    return m_currentAnimation;
}

int AnimationComponent::getCurrentFrame() const {
    return m_currentFrame;
}

void AnimationComponent::reset() {
    m_currentFrame = 0;
    m_frameTimer = 0.0f;
    m_playing = true;
    updateSpriteRect();
}

void AnimationComponent::updateSpriteRect() {
    if (m_currentAnimation.empty()) {
        return;
    }

    auto it = m_animations.find(m_currentAnimation);
    if (it == m_animations.end()) {
        return;
    }

    const AnimationData &anim = it->second;

    // 计算纹理区域（横向排列的精灵表）
    sf::IntRect rect(
        sf::Vector2i(m_currentFrame * anim.frameWidth, 0),
        sf::Vector2i(anim.frameWidth, anim.frameHeight));

    m_sprite.setTextureRect(rect);

    // 设置原点为底部中心
    m_sprite.setOrigin(sf::Vector2f(anim.frameWidth / 2.0f, anim.frameHeight));
}

} // namespace SamuraiFight