#pragma once

#include <SFML/Audio.hpp>

class Sound {
    sf::Sound m_sound;
    sf::SoundBuffer m_sound_buffer;

public:
    bool loadFromFile(const std::string& path);

    void play();
};
