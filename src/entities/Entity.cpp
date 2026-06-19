#include "Entity.h"
#include "../core/Constants.h"

namespace SamuraiFight {

Entity::Entity()
    : m_position(0.0f, GROUND_LEVEL)
    , m_velocity(0.0f, 0.0f)
    , m_onGround(true) {
}

Entity::~Entity() {
}

sf::Vector2f Entity::getPosition() const {
    return m_position;
}

void Entity::setPosition(const sf::Vector2f& position) {
    m_position = position;
}

void Entity::move(const sf::Vector2f& offset) {
    m_position += offset;
}

sf::Vector2f Entity::getVelocity() const {
    return m_velocity;
}

void Entity::setVelocity(const sf::Vector2f& velocity) {
    m_velocity = velocity;
}

bool Entity::isOnGround() const {
    return m_onGround;
}

} // namespace SamuraiFight