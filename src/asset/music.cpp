#include "music.hpp"

bool Music::openFromFile(const std::string& path) {
    return m_music.openFromFile(path);
}

void Music::setVolume(float volume) {
    m_music.setVolume(volume);
}

void Music::setLoop(bool loop){
    m_music.setLooping(loop);
}

void Music::play() {
    m_music.play();
}

void Music::stop() {
    m_music.stop();
}
