#include "sound.hpp"
#include <iostream>

bool Sound::loadFromFile(const std::string& path) {
    auto loaded = m_sound_buffer.loadFromFile(path);
    return loaded;
}
 
void Sound::play() {
    if (m_sound.has_value()) {
        m_sound->play();
    }
}
