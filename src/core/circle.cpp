#include "circle.hpp"

#include "../asset/color.hpp"
#include "../math/vec2.hpp"

Circle::Circle(float r)
    : m_circle(r) {
}

void Circle::setFillColor(Color color) {
    m_circle.setFillColor(color.getColor());
}

void Circle::setRadius(float r) {
    m_circle.setRadius(r);
}

void Circle::setOrigin(float x, float y) {
    m_circle.setOrigin({x, y});
}

void Circle::setOrigin(const Vec2& origin) {
    m_circle.setOrigin({origin.x, origin.y});
}

void Circle::setPosition(float x, float y) {
    m_circle.setPosition({x, y});
}

void Circle::setPosition(const Vec2& pos) {
    m_circle.setPosition({pos.x, pos.y});
}
