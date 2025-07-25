#pragma once

#include <SFML/Graphics.hpp>

class Vec2;

// Wrapper for view
class View {
    sf::View m_view;

public:
    View() = default;

    View(const sf::View& view)
        : m_view(view)
    {
    }

    View(float rect_left, float rect_top, float width, float height);

    Vec2 getCenter() const;

    const sf::View& getView() const { return m_view; }

    void setCenter(float x, float y);
    void setCenter(const Vec2& pos);
    void zoom(float factor);
};
