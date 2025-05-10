#pragma once

#include <SFML/Audio.hpp>

// Wrapper for sound buffer
class SoundBuffer {
    sf::SoundBuffer m_sound_buffer;

public:
    bool loadFromFile(const std::string& path);

    const sf::SoundBuffer& getSoundBuffer() const { return m_sound_buffer; }
};
