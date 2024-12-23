#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "math/vec2.hpp"

// Used for parallax scrolling
class Layer {
    sf::View m_view;
    std::string m_name;
    sf::Sprite m_sprite;
    Vec2 m_size{ 0, 0 };
    float m_view_offset_x{ 0.0f };
    float m_sprite_offset_x{ 0.0f };

public:
    Layer() = default;
    Layer(const std::string& name, const sf::Texture& texture);

    void update(float velocity_x, float coeff = 0.025f);
    void scale(size_t window_w, size_t window_h);

    sf::Sprite& getSprite() { return m_sprite; }
    const sf::View& getView() const { return m_view; }
    const std::string& getName() const { return m_name; }
};
