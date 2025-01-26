#include "sprite.hpp"
#include "texture.hpp"

Sprite::Sprite(const Texture& texture)
    : m_sprite(texture.getTexture())
{
}

const sf::IntRect& Sprite::getTextureRect() const {
    return m_sprite.getTextureRect();
}

void Sprite::setOrigin(float x, float y) {
    m_sprite.setOrigin(x, y);
}

void Sprite::setTextureRect(int left, int top, int width, int height) {
    m_sprite.setTextureRect({
        left, top, width, height
    });
}

void Sprite::setRotation(float angle) {
    m_sprite.setRotation(angle);
}

void Sprite::setPosition(float x, float y) {
    m_sprite.setPosition(x, y);
}

void Sprite::setScale(float scale_x, float scale_y) {
    m_sprite.setScale(scale_x, scale_y);
}

void Sprite::scale(float scale_x, float scale_y) {
    m_sprite.scale(scale_x, scale_y);
}

void Sprite::setColor(channel r, channel g, channel b, channel a) {
    m_sprite.setColor(sf::Color(r, g, b, a));
}
