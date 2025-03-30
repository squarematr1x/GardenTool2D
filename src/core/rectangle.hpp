#pragma once

#include "SFML/Graphics.hpp"

struct Color;

class Rectangle {
    sf::RectangleShape m_rectangle;

public:
    void setFillColor(const Color& color);
    void setPosition(float x, float y);
    void setSize(float width, float height);

    const sf::RectangleShape& getRectangle() const { return m_rectangle; }
};
