#pragma once

#include <SFML/Graphics.hpp>

// Wrapper for font
class Font {
    sf::Font m_font;

public:
    bool loadFromFile(const std::string& path);

    const sf::Font& getFont() const { return m_font; }
};
