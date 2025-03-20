#include "texture.hpp"

#include "../math/vec2.hpp"

const Vec2 Texture::getSize() const {
    const auto size = m_texture.getSize();
    return Vec2(size.x, size.y);
}

void Texture::setRepeated(bool repeated) {
    m_texture.setRepeated(repeated);
}

void Texture::setSmooth(bool smooth) {
    m_texture.setSmooth(smooth);
}

void Texture::update(const sf::Window& window) {
    m_texture.update(window);
}

bool Texture::loadFromFile(const std::string& path){
    return m_texture.loadFromFile(path);
}

bool Texture::create(unsigned int width, unsigned int height) {
    return m_texture.create(width, height);
}

bool Texture::create(float width, float height) {
    return m_texture.create(
        static_cast<unsigned int>(width),
        static_cast<unsigned int>(height)
    );
}

bool Texture::saveToFile(const std::string& path) {
    const auto image = m_texture.copyToImage();
    return image.saveToFile(path);
}
