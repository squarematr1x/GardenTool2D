#include "sound.hpp"
#include <iostream>

bool Sound::loadFromFile(const std::string& path) {
    auto loaded = m_sound_buffer.loadFromFile(path);
    return loaded;
}
 
void Sound::play() {
    m_sound.setBuffer(m_sound_buffer);
    m_sound.play();
}
