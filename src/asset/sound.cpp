#include "sound.hpp"
#include "sound-buffer.hpp"

#include <iostream>

Sound::Sound(const SoundBuffer& sound_buffer)
    : m_sound(sound_buffer.getSoundBuffer())
{
}

void Sound::play() {
    m_sound.play();
}
