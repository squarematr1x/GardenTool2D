#include "layer.hpp"

Layer::Layer(const std::string& name, const Texture& texture)
    : m_name(name), m_sprite(texture.getTexture()), m_size(texture.getSize()) {
    m_sprite.setTextureRect(sf::IntRect(0, 0, 3*static_cast<int>(m_size.x), 2*static_cast<int>(m_size.y)));
    m_sprite.setOrigin(m_size.x, m_size.y);
}

void Layer::update(float velocity_x, float coeff) {
    m_view.setCenter(m_view_offset_x, -m_size.y);
    m_view_offset_x += velocity_x*coeff;
    m_sprite_offset_x = floor(m_view_offset_x/m_size.x)*m_size.x;
    m_sprite.setPosition(m_sprite_offset_x, m_size.y);
}   

void Layer::scale(size_t window_w, size_t window_h) {
    m_sprite.scale(
        window_w/static_cast<unsigned int>(m_size.x),
        window_h/static_cast<unsigned int>(m_size.y)
    );
}
