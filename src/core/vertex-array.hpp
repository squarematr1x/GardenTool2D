#pragma once

#include "SFML/Graphics.hpp"

#include "../asset/color.hpp"
#include "../math/vec2.hpp"
#include "primitive.hpp"

// NOTE: might be used directly later
struct Vertex {
    Vec2 pos;
    Vec2 tex_coord;
    Color color;

    Vertex() = default;
    Vertex(const Vec2& pos_in, const Vec2& tex_coord_in, const Color& color_in)
        : pos(pos_in), tex_coord(tex_coord_in), color(color_in)
    {
    }
    Vertex(const Vec2& pos_in, const Color& color_in)
        : pos(pos_in), color(color_in)
    {
    }
};

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
