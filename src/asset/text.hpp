#pragma once

#include <SFML/Graphics.hpp>

#include "rect.hpp"

class Font;
class Vec2;

using channel = unsigned char;

class Text {
    // Wrapper for text
    sf::Text m_text;

public:
    Text() = default;
    Text(const Font& font, const std::string& name, unsigned int char_size, Vec2 pos);
    Text(const Font& font, const std::string& name, unsigned int char_size, float x, float y);

    const sf::Text& getText() const { return m_text; }
    Rect<float> getLocalBounds();

    void setFont(const Font& font);
    void setString(const std::string& name);
    void setCharacterSize(unsigned int char_size);
    void setPosition(float x, float y);
    void setOrigin(float x, float y);
    void setFillColor(channel r, channel g, channel b, channel a = 255);
};
