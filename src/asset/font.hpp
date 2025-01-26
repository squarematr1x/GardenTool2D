#pragma once

#include <SFML/Graphics.hpp>

class Font {
    // Wrapper for font
    sf::Font m_font;

public:
    bool loadFromFile(const std::string& path);

    const sf::Font& getFont() const { return m_font; }
};
