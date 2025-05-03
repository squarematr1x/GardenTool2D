#pragma once

#include <SFML/Graphics.hpp>

#include "../core/rect.hpp"

class Texture;
struct Color;

// Wrapper for sprite
class TextureRect {
    sf::RectangleShape m_tex_rect;

public:
    TextureRect() = default;
    TextureRect(const Texture& texture);

    const sf::RectangleShape& getSprite() const { return m_tex_rect; }
    const Rect<float> getTextureRect() const;

    void setOrigin(float x, float y);
    void setTextureRect(int left, int top, int width, int height);
    void setRotation(float angle);
    void setPosition(float x, float y);
    void setScale(float scale_x, float scale_y);
    void scale(float scale_x, float scale_y);

    void setColor(const Color& color);
};
