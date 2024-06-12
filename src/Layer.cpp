#include "layer.hpp"

Layer::Layer(const std::string& name, const sf::Texture& texture, Vec2 size, Vec2 offset)
    : m_name(name), m_sprite(texture), m_size(size), m_offset(offset), m_x_pos(offset.x) {
    m_sprite.setTextureRect(sf::IntRect(
        static_cast<int>(m_offset.x),
        static_cast<int>(m_offset.y),
        static_cast<int>(m_size.x),
        static_cast<int>(m_size.y)
    ));
}

void Layer::update(float velocity_x, float coeff) {
    m_x_pos += velocity_x * coeff;
    m_sprite.setTextureRect(sf::IntRect(
        static_cast<int>(m_offset.x) + static_cast<int>(m_x_pos) % 192, // TODO: remove hard coded values
        static_cast<int>(m_offset.y),
        static_cast<int>(m_size.x),
        static_cast<int>(m_size.y)
    ));
}
