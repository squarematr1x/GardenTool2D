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

    const sf::Color& getColor() const { return m_color; } 
};
