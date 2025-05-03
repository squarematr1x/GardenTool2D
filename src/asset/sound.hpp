#pragma once

#include <SFML/Audio.hpp>

// Wrapper for sound
class Sound {
    std::optional<sf::Sound> m_sound;
    sf::SoundBuffer m_sound_buffer;

public:
    Sound() = default;

    bool loadFromFile(const std::string& path);

    void play();
};
