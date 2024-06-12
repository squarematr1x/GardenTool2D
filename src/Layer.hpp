#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "math/vec2.hpp"

// Used for parallax scrolling
class Layer {
    std::string m_name;
    sf::Sprite m_sprite;
    Vec2 m_size{ 0, 0 };
    Vec2 m_offset{ 0, 0 }; // Offset in texture atlas
    float m_x_pos{ 0.0f };

public:
    Layer() = default;
    Layer(const std::string& name, const sf::Texture& texture, Vec2 size, Vec2 offset);

    void update(float velocity_x, float coeff = 0.025f);

    sf::Sprite& getSprite() { return m_sprite; }
};
