#include "render-window.hpp"

#include "event.hpp"
#include "../math/vec2.hpp"

void RenderWindow::create(unsigned int w, unsigned int h, const std::string& title) {
    m_window.create(sf::VideoMode(w, h), title);
}

void RenderWindow::setFramerateLimit(unsigned int limit) {
    m_window.setFramerateLimit(limit);
}

void RenderWindow::setVerticalSyncEnabled(bool enable) {
    m_window.setVerticalSyncEnabled(enable);
}

void RenderWindow::setView(const View& view) {
    (void)view;
    // m_window.setView(view.getView());
}

void RenderWindow::setDefaultView() {
    m_window.setView(m_window.getDefaultView());
}

void RenderWindow::draw(
    const sf::Drawable& drawable) {
    m_window.draw(drawable);
}

void RenderWindow::display() {
    m_window.display();
}

void RenderWindow::close() {
    m_window.close();
}

void RenderWindow::clear(unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
    m_window.clear({r, g, b, a});
}

bool RenderWindow::isOpen() const {
    return m_window.isOpen();
}

bool RenderWindow::pollEvent(Event& event) {
    return m_window.pollEvent(event.getEvent());
}

Vec2 RenderWindow::getSize() const {
    return Vec2(m_window.getSize().x, m_window.getSize().y);
}
