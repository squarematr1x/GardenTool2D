#include "font.hpp"

bool Font::loadFromFile(const std::string& path) {
    return m_font.openFromFile(path);
}
