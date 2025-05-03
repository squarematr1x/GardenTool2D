#include "rectangle.hpp"

#include "../asset/color.hpp"

void Rectangle::setFillColor(const Color& color) {
    m_rectangle.setFillColor(color.getColor());
}

void Rectangle::setPosition(float x, float y) {
    m_rectangle.setPosition({x, y});
}

void Rectangle::setSize(float width, float height) {
    m_rectangle.setSize({ width, height });
}
