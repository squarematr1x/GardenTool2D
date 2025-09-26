#pragma once

#include "SFML/Graphics.hpp"

struct Color;

class Vec2;

// Wrapper for circle
class Circle {
    sf::CircleShape m_circle;

public:
    Circle() = default;
    Circle(float r);

    void setFillColor(Color color);
    void setRadius(float r);
    void setOrigin(float x, float y);
    void setOrigin(const Vec2& origin);
    void setPosition(float x, float y);
    void setPosition(const Vec2& pos);

    const sf::CircleShape& getCircle() const { return m_circle; }
};
