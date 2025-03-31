#pragma once

#include <SFML/Graphics.hpp>

#include "../core/rect.hpp"

class Texture;
struct Color;

class Sprite {
    // Wrapper for sprite
    sf::Sprite m_sprite;

public:
    Sprite() = default;
    Sprite(const Texture& texture);

    const sf::Sprite& getSprite() const { return m_sprite; }
    const Rect<float> getTextureRect() const;

    void setOrigin(float x, float y);
    void setTextureRect(int left, int top, int width, int height);
    void setRotation(float angle);
    void setPosition(float x, float y);
    void setScale(float scale_x, float scale_y);
    void scale(float scale_x, float scale_y);

    void setColor(const Color& color);
};
