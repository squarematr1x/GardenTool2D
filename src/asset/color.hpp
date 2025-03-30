#pragma once

#include <SFML/Graphics.hpp>

using channel = unsigned char;

struct Color {
    sf::Color m_color{ 255, 255, 255 , 255 };

    Color() = default;

    Color(channel r, channel g, channel b, channel a=255)
        : m_color(sf::Color(r, g, b, a))
    {
    }

    void setR(channel r) { m_color.r= r; }
    void setG(channel g) { m_color.g = g; }
    void setB(channel b) { m_color.b = b; }
    void setA(channel a) { m_color.a = a; }

    const sf::Color& getColor() const { return m_color; } 
};
