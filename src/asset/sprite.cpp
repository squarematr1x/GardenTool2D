#include "sprite.hpp"
#include "texture.hpp"
#include "color.hpp"

Sprite::Sprite(const Texture& texture)
    : m_sprite(texture.getTexture())
{
}

const Rect<float> Sprite::getTextureRect() const {
    auto rect = m_sprite.getTextureRect();
    return Rect<float>(
        static_cast<float>(rect.position.x),
        static_cast<float>(rect.position.y),
        static_cast<float>(rect.size.x),
        static_cast<float>(rect.size.y)
    );
}

void Sprite::setOrigin(float x, float y) {
    m_sprite.setOrigin({x, y});
}

void Sprite::setTextureRect(int left, int top, int width, int height) {
    m_sprite.setTextureRect({
        {left, top}, {width, height}
    });
}

void Sprite::setRotation(float angle) {
    m_sprite.setRotation(sf::degrees(angle));
}

void Sprite::setPosition(float x, float y) {
    m_sprite.setPosition({x, y});
}

void Sprite::setScale(float scale_x, float scale_y) {
    m_sprite.setScale({scale_x, scale_y});
}

void Sprite::scale(float scale_x, float scale_y) {
    m_sprite.scale({scale_x, scale_y});
}

void Sprite::setColor(const Color& color) {
    m_sprite.setColor(color.getColor());
}
