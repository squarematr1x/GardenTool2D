#pragma once

#include <SFML/Audio.hpp>

// Wrapper for music
class Music {
    sf::Music m_music;

public:
    bool openFromFile(const std::string& path);

    void setVolume(float volume);
    void setLoop(bool loop);
    void play();
    void stop();
};
