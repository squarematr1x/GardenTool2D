#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "math/Vec2.hpp"

class Layer {
    std::string m_name;
    sf::Sprite m_sprite;
    Vec2 m_size{ 0, 0};
    Vec2 m_offset{ 0, 0};

public:
    Layer() = default;
    Layer(const std::string& name, const sf::Texture& texture, Vec2 size, Vec2 offset);

    void update(Vec2 velocity);

    sf::Sprite& getSprite() { return m_sprite; }
};
