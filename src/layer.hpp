#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "math/vec2.hpp"
#include "asset/texture.hpp"
#include "asset/sprite.hpp"

// Used for parallax scrolling
class Layer {
    sf::View m_view;
    std::string m_name;
    Sprite m_sprite;
    Vec2 m_size{ 0, 0 };
    float m_view_offset_x{ 0.0f };
    float m_sprite_offset_x{ 0.0f };

public:
    Layer() = default;
    Layer(const std::string& name, const Texture& texture);

    void init(const sf::RenderWindow& window);
    void update(float velocity_x, float coeff, const sf::RenderWindow& window);

    Sprite& getSprite() { return m_sprite; }
    const sf::View& getView() const { return m_view; }
    const std::string& getName() const { return m_name; }
};
