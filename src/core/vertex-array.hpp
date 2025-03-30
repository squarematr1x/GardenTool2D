#pragma once

#include "SFML/Graphics.hpp"

#include "../asset/color.hpp"
#include "primitive.hpp"

class Vec2;

class VertexArray {
    sf::VertexArray m_vertex_array;

public:
    VertexArray() = default;
    VertexArray(Primitive primitive, size_t vertex_count=0UL);

    const sf::VertexArray& getVertexArray() const { return m_vertex_array; }
    sf::VertexArray& getVertexArray() { return m_vertex_array; }

    void append(const Vec2& pos, const Vec2& tex_coord);
    void append(const Vec2& pos, const Vec2& tex_coord, const Color& color);
    void append(const Vec2& pos, const Color& color);

    void setPosAt(size_t index, const Vec2& pos);
    void incPosAt(size_t index, const Vec2& pos);
    void setColorAt(size_t index, const Color& color);
    void setVertexAt(size_t index, const Vec2& pos, const Color& color);

    void clear();
};
