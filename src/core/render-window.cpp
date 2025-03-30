#include "render-window.hpp"

#include "event.hpp"
#include "../math/vec2.hpp"
#include "../core/vertex-array.hpp"
#include "../core/circle.hpp"
#include "../core/rectangle.hpp"
#include "../asset/texture.hpp"

void RenderWindow::create(unsigned int w, unsigned int h, const std::string& title) {
    m_window.create(sf::VideoMode(w, h), title);
    m_view = View(m_window.getView());
}

void RenderWindow::setFramerateLimit(unsigned int limit) {
    m_window.setFramerateLimit(limit);
}

void RenderWindow::setVerticalSyncEnabled(bool enable) {
    m_window.setVerticalSyncEnabled(enable);
}

void RenderWindow::setView(const View& view) {
    m_view = view;
    m_window.setView(view.getView());
}

void RenderWindow::setDefaultView() {
    m_window.setView(m_window.getDefaultView());
}

void RenderWindow::draw(const sf::Drawable& drawable) {
    m_window.draw(drawable);
}

void RenderWindow::draw(const VertexArray& vertex_array) {
    m_window.draw(vertex_array.getVertexArray());
}

void RenderWindow::draw(const VertexArray& vertex_array, const Texture& texture) {
    sf::RenderStates states(&texture.getTexture());
    m_window.draw(vertex_array.getVertexArray(), states);
}

void RenderWindow::draw(const Circle& circle) {
    m_window.draw(circle.getCircle());
}

void RenderWindow::draw(const Rectangle& rectangle) {
    m_window.draw(rectangle.getRectangle());
}

void RenderWindow::display() {
    m_window.display();
}

void RenderWindow::close() {
    m_window.close();
}

void RenderWindow::clear(channel r, channel g, channel b, channel a) {
    m_window.clear({r, g, b, a});
}

bool RenderWindow::isOpen() const {
    return m_window.isOpen();
}

bool RenderWindow::pollEvent(Event& event) {
    return m_window.pollEvent(event.getEvent());
}

unsigned int RenderWindow::width() const {
    return m_window.getSize().x;
}

unsigned int RenderWindow::heigh() const {
    return m_window.getSize().y;
}

Vec2 RenderWindow::getSize() const {
    return Vec2(m_window.getSize().x, m_window.getSize().y);
}

Vec2 RenderWindow::mapPixelToCoords(const Vec2 pos) const {
    auto coords = m_window.mapPixelToCoords({
        static_cast<int>(pos.x),
        static_cast<int>(pos.y)
    });
    return Vec2(coords.x, coords.y);
}


const View& RenderWindow::getView() const {
    return m_view;
}

const sf::View& RenderWindow::getDefaultView() const {
    return m_window.getDefaultView();
}
