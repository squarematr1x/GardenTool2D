#include "circle.hpp"
#include "../asset/color.hpp"

void Circle::setFillColor(Color color) {
    m_circle.setFillColor(color.getColor());
}

void Circle::setRadius(float r) {
    m_circle.setRadius(r);
}

void Circle::setOrigin(float x, float y) {
    m_circle.setOrigin(x, y);
}

void Circle::setPosition(float x, float y) {
    m_circle.setPosition(x, y);
}
