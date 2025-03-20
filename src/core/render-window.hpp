#pragma once

#include <SFML/Graphics.hpp>

#include "view.hpp"

class View;
class Event;
class Vec2;

using channel = unsigned char;

class RenderWindow {
    sf::RenderWindow m_window;
    View m_view;

public:
    RenderWindow() = default;

    void create(unsigned int w, unsigned int h, const std::string& title);

    void setFramerateLimit(unsigned int limit);
	void setVerticalSyncEnabled(bool enable);
    void setView(const View& view);
    void setDefaultView();

    void draw(const sf::Drawable& drawable);
    void display();
    void close();
    void clear(channel r, channel g, channel b, channel a=255);

    bool isOpen() const;
    bool pollEvent(Event& event);

    Vec2 getSize() const;

    const View& getView() const;

    const sf::View& getDefaultView() const;

    sf::RenderWindow& getWindow() { return m_window; }
    const sf::RenderWindow& getWindow() const { return m_window; }
};
