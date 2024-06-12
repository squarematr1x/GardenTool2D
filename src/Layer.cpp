#include "Layer.hpp"

Layer::Layer(const std::string& name, const sf::Texture& texture, Vec2 size, Vec2 offset)
    : m_name(name), m_sprite(texture), m_size(size), m_offset(offset) {
    m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
    m_sprite.setTextureRect(sf::IntRect(
        static_cast<int>(m_offset.x),
        static_cast<int>(m_offset.y),
        static_cast<int>(m_size.x),
        static_cast<int>(m_size.y)
    ));
}

void Layer::update(Vec2 velocity) {
    m_sprite.setTextureRect(sf::IntRect(
        static_cast<int>(m_offset.x + velocity.x),
        static_cast<int>(m_offset.y),
        static_cast<int>(m_size.x),
        static_cast<int>(m_size.y)
    ));
}
