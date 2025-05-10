#include "sound-buffer.hpp"

bool SoundBuffer::loadFromFile(const std::string& path) {
    return m_sound_buffer.loadFromFile(path);
}
