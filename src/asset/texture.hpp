#pragma once

#include <SFML/Graphics.hpp>

class Vec2;

// Wrapper for texture
class Texture {
    sf::Texture m_texture;

public:
    const sf::Texture& getTexture() const { return m_texture; }

    const Vec2 getSize() const;

    void setRepeated(bool repeated);
    void setSmooth(bool smooth);
    void update(const sf::Window& window);

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path);
    bool create(unsigned int width, unsigned int height);
    bool create(float width, float height);
};
