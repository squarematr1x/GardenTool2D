#include "vertex-array.hpp"

#include "../math/vec2.hpp"

VertexArray::VertexArray(Primitive primitive, size_t vertex_count)
    : m_vertex_array(static_cast<sf::PrimitiveType>(primitive), vertex_count)
{
}

void VertexArray::append(const Vec2& pos, const Vec2& tex_coord) {
    sf::Vertex vertex;
    vertex.position = { pos.x, pos.y };
    vertex.texCoords = {tex_coord.x, tex_coord.y};
    m_vertex_array.append(vertex);
}

void VertexArray::append(const Vec2& pos, const Vec2& tex_coord, const Color& color) {
    m_vertex_array.append({{pos.x, pos.y}, color.getColor(), {tex_coord.x, tex_coord.y}});
}

void VertexArray::append(const Vec2& pos, const Color& color) {
    m_vertex_array.append({{pos.x, pos.y}, color.getColor()});
}

void VertexArray::setPosAt(size_t index,const Vec2& pos) {
    m_vertex_array[index].position = { pos.x, pos.y };
}

void VertexArray::incPosAt(size_t index, const Vec2& pos) {
    m_vertex_array[index].position.x += pos.x;
    m_vertex_array[index].position.y += pos.y;
}

void VertexArray::setColorAt(size_t index,  const Color& color) {
    m_vertex_array[index].color = color.getColor();
}

void VertexArray::setVertexAt(size_t index, const Vec2& pos, const Color& color) {
    m_vertex_array[index].position.x += pos.x;
    m_vertex_array[index].position.y += pos.y;
    m_vertex_array[index].color = color.getColor();
}

void VertexArray::clear() {
    m_vertex_array.clear();
}
