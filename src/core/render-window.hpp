#pragma once

#include <SFML/Graphics.hpp>

class View;
class Event;
class Vec2;

class RenderWindow {
    sf::RenderWindow m_window;

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
    void clear(unsigned int r, unsigned int g, unsigned int b, unsigned int a=255);

    bool isOpen() const;
    bool pollEvent(Event& event);

    Vec2 getSize() const;

    sf::RenderWindow& getWindow() { return m_window; }
    const sf::RenderWindow& getWindow() const { return m_window; }
};
