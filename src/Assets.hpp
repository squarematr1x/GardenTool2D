#pragma once

#include <map>
#include <string>
#include <memory>

#include <SFML/Audio.hpp>

#include "Animation.hpp"

class Assets
{
	// TODO: Prefer enums over strings?
	std::map<std::string, sf::Texture> m_texture_map;
	std::map<std::string, Animation> m_animation_map;
	std::map<std::string, sf::Font> m_font_map;
	std::map<std::string, sf::SoundBuffer> m_sound_buffer_map;
	std::map<std::string, sf::Sound> m_sound_map;
	std::map<std::string, std::shared_ptr<sf::Music>> m_music_map;

	void addTexture(const std::string& texture_name, const std::string& path, bool smooth = true);
	void addAnimation(const std::string& animation_name, const std::string& texture_name, size_t frame_count, int speed);
	void addFont(const std::string& font_name, const std::string& path);
	void addSoundBuffer(const std::string& sound_name, const std::string& path);
	void addSound(const std::string& sound_name, const std::string& path);
	void addMusic(const std::string& music_name, const std::string& path);

	const sf::SoundBuffer& getSoundBuffer(const std::string& sound_name) const;

public:
	void loadFromFile(const std::string& path);

	const sf::Texture& getTexture(const std::string& texture_name) const;
	const Animation& getAnimation(const std::string& animation_name) const;
	const sf::Font& getFont(const std::string& font_name) const;
	sf::Sound getSound(const std::string& sound_name) const;
	const std::shared_ptr<sf::Music> getMusic(const std::string& music_name) const;
};
