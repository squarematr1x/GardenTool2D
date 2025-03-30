#pragma once

#include <SFML/Graphics.hpp>

#include "rect.hpp"

class Texture;

using channel = unsigned char;

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

    // TODO: use Color struct here
    void setColor(channel r, channel g, channel b, channel a=255);
};
