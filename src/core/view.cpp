#include "view.hpp"

#include "../math/vec2.hpp"

View::View(float rect_left, float rect_top, float width, float height)
    : m_view({
        rect_left, rect_top, width, height
    })
{
}

void View::setCenter(float x, float y) {
    m_view.setCenter({x, y});
}

void View::zoom(float factor) {
    m_view.zoom(factor);
}

Vec2 View::getCenter() const {
    const auto center = m_view.getCenter();
    return Vec2(center.x, center.y);
}
