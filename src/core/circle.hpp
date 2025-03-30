#pragma once

#include "SFML/Graphics.hpp"

struct Color;

class Circle {
    sf::CircleShape m_circle;

public:
    Circle() = default;

    void setFillColor(Color color);
    void setRadius(float r);
    void setOrigin(float x, float y);
    void setPosition(float x, float y);

    const sf::CircleShape& getCircle() const { return m_circle; }
};
