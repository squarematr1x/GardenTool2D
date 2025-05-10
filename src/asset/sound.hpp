#pragma once

#include <SFML/Audio.hpp>

class SoundBuffer;

// Wrapper for sound
class Sound {
    sf::Sound m_sound;

public:
    Sound(const SoundBuffer& sound_buffer);

    void play();
};
