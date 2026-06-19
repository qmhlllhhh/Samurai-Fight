#include "HitboxComponent.h"
#include "../core/Constants.h"

namespace SamuraiFight {

bool Hitbox::isActive() const {
    if (activeFrames.empty()) {
        return true; // 没有指定激活帧，始终激活
    }

    for (int frame : activeFrames) {
        if (currentFrame == frame) {
            return true;
        }
    }
    return false;
}

HitboxComponent::HitboxComponent()
    : m_facingRight(true) {
}

HitboxComponent::~HitboxComponent() {
}

void HitboxComponent::loadHitboxes(const std::string& state, const std::vector<HitboxData>& hitboxData) {
    std::vector<Hitbox> hitboxes;

    for (const auto& data : hitboxData) {
        Hitbox hitbox;
        hitbox.rect = sf::FloatRect(
            sf::Vector2f(data.x, data.y),
            sf::Vector2f(data.width, data.height)
        );
        hitbox.type = data.type;
        hitbox.activeFrames = data.activeFrames;
        hitbox.currentFrame = 0;
        hitboxes.push_back(hitbox);
    }

    m_hitboxesByState[state] = hitboxes;
}

void HitboxComponent::update(const sf::Vector2f& position, const std::string& state, int frame, bool facingRight) {
    m_position = position;
    m_facingRight = facingRight;

    // 获取当前状态的碰撞框
    auto it = m_hitboxesByState.find(state);
    if (it == m_hitboxesByState.end()) {
        // 如果没有找到，使用默认碰撞框
        m_currentHitboxes.clear();
        return;
    }

    m_currentHitboxes = it->second;

    // 更新碰撞框位置
    for (auto& hitbox : m_currentHitboxes) {
        hitbox.currentFrame = frame;

        // 根据朝向调整位置
        if (m_facingRight) {
            hitbox.rect.position.x = m_position.x + hitbox.rect.position.x;
        } else {
            // 朝左时镜像碰撞框
            hitbox.rect.position.x = m_position.x - hitbox.rect.position.x - hitbox.rect.size.x;
        }
        hitbox.rect.position.y = m_position.y + hitbox.rect.position.y;
    }
}

std::vector<Hitbox> HitboxComponent::getHurtboxes() const {
    std::vector<Hitbox> hurtboxes;
    for (const auto& hitbox : m_currentHitboxes) {
        if (hitbox.type == "hurtbox" && hitbox.isActive()) {
            hurtboxes.push_back(hitbox);
        }
    }
    return hurtboxes;
}

std::vector<Hitbox> HitboxComponent::getHitboxes() const {
    std::vector<Hitbox> hitboxes;
    for (const auto& hitbox : m_currentHitboxes) {
        if (hitbox.type == "hitbox" && hitbox.isActive()) {
            hitboxes.push_back(hitbox);
        }
    }
    return hitboxes;
}

bool HitboxComponent::intersects(const Hitbox& other) const {
    for (const auto& hitbox : m_currentHitboxes) {
        if (hitbox.isActive() && hitbox.rect.findIntersection(other.rect)) {
            return true;
        }
    }
    return false;
}

void HitboxComponent::renderDebug(sf::RenderWindow& window) {
    for (const auto& hitbox : m_currentHitboxes) {
        if (!hitbox.isActive()) continue;

        sf::RectangleShape shape;
        shape.setPosition(hitbox.rect.position);
        shape.setSize(hitbox.rect.size);

        if (hitbox.type == "hurtbox") {
            shape.setFillColor(sf::Color(0, 255, 0, 50));  // 绿色半透明
            shape.setOutlineColor(sf::Color(0, 255, 0));
        } else {
            shape.setFillColor(sf::Color(255, 0, 0, 50));  // 红色半透明
            shape.setOutlineColor(sf::Color(255, 0, 0));
        }
        shape.setOutlineThickness(2.0f);

        window.draw(shape);
    }
}

} // namespace SamuraiFight