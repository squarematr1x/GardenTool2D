#include "layer.hpp"

#include <iostream>

Layer::Layer(const std::string& name, const Texture& texture)
    : m_name(name), m_sprite(texture), m_size(texture.getSize()) {
}

void Layer::init(const sf::RenderWindow& window) {
    m_view = sf::View(window.getDefaultView());
    m_sprite.setTextureRect(
        0,
        0,
        static_cast<int>(m_size.x*8),
        static_cast<int>(m_size.y)
    );
    m_sprite.setOrigin(m_size.x*4, m_size.y);
    m_sprite.setScale(
        1.0f,
        static_cast<float>(window.getSize().y)/m_size.y
    );
}

void Layer::update(float velocity_x, float coeff, const sf::RenderWindow& window) {
    m_view.setCenter(m_view_offset_x, 0);
    m_view_offset_x += velocity_x*coeff;
    m_sprite_offset_x = floor(m_view_offset_x/m_size.x)*m_size.x;
    m_sprite.setPosition(m_sprite_offset_x, window.getSize().y/2);
}
