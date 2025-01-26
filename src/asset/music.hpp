#pragma once

#include <SFML/Audio.hpp>

class Music {
    // Wrapper for music
    sf::Music m_music;

public:
    bool openFromFile(const std::string& path);

    void setVolume(float volume);
    void setLoop(bool loop);
    void play();
    void stop();
};
