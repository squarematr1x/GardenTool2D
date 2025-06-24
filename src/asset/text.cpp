#include "text.hpp"

#include "font.hpp"
#include "color.hpp"
#include "../math/vec2.hpp"

Text::Text(const Font& font, const std::string& name, unsigned int char_size, Vec2 pos)
    : m_text(font.getFont(), name, char_size) {
    m_text.setPosition({pos.x, pos.y});
}

Text::Text(const Font& font, const std::string& name, unsigned int char_size, float x, float y)
    : m_text(font.getFont(), name, char_size) {
    m_text.setPosition({x, y});
}

const Rect<float> Text::getLocalBounds() const {
    const auto local_bounds = m_text.getLocalBounds();
    const Rect<float> rectangle(
        local_bounds.position.x,
        local_bounds.position.y,
        local_bounds.size.x,
        local_bounds.size.y
    );
    return rectangle;
}

void Text::setFont(const Font& font) {
    m_text.setFont(font.getFont());
}

void Text::setString(const std::string& name) {
    m_text.setString(name);
}

void Text::setCharacterSize(unsigned int char_size) {
    m_text.setCharacterSize(char_size);
}

void Text::setPosition(float x, float y) {
    m_text.setPosition({x, y});
}

void Text::setOrigin(float x, float y) {
    m_text.setOrigin({x, y});
}

void Text::setFillColor(const Color& color) {
    m_text.setFillColor(color.getColor());
}
