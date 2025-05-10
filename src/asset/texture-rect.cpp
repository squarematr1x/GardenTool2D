#include "texture-rect.hpp"
#include "texture.hpp"
#include "color.hpp"
#include "../math/vec2.hpp"

TextureRect::TextureRect(const Texture& texture) {
    m_tex_rect.setTexture(texture.getTexturePtr());
}

const Rect<float> TextureRect::getTextureRect() const {
    auto rect = m_tex_rect.getTextureRect();
    return Rect<float>(
        static_cast<float>(rect.position.x),
        static_cast<float>(rect.position.y),
        static_cast<float>(rect.size.x),
        static_cast<float>(rect.size.y)
    );
}

void TextureRect::setOrigin(float x, float y) {
    m_tex_rect.setOrigin({x, y});
}

void TextureRect::setTextureRect(int left, int top, int width, int height) {
    m_tex_rect.setTextureRect({
        {left, top}, {width, height}
    });
}

void TextureRect::setRotation(float angle) {
    m_tex_rect.setRotation(sf::degrees(angle));
}

void TextureRect::setPosition(float x, float y) {
    m_tex_rect.setPosition({x, y});
}

void TextureRect::setScale(float scale_x, float scale_y) {
    m_tex_rect.setScale({scale_x, scale_y});
}

void TextureRect::scale(float scale_x, float scale_y) {
    m_tex_rect.scale({scale_x, scale_y});
}

void TextureRect::setSize(float x, float y) {
    m_tex_rect.setSize({x, y});
}

void TextureRect::setColor(const Color& color) {
    m_tex_rect.setFillColor(color.getColor());
}
