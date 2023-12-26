#pragma once

#include "Animation.h"

// FIXME: double includes etc.

#include "Components.h"

#include <SFML/Audio.hpp>

#include <map>
#include <string>

// Assets are: textures, animations, sounds, fonts

class Assets
{
	std::map<std::string, sf::Texture> m_texture_map;
	std::map<std::string, Animation> m_animation_map;
	std::map<std::string, sf::Font> m_font_map;
	std::map<std::string, sf::Sound> m_sound_map;

	void addTexture(const std::string& texture_name, const std::string& path, bool smooth = true);
	void addAnimation(const std::string& animation_name, const std::string& texture_name, int frame_count, int speed);
	void addFont(const std::string& font_name, const std::string& path);
	void addSound(const std::string& sound_name, const std::string& path);

public:
	Assets();

	void loadFromFile(const std::string& path);

	const sf::Texture& getTexture(const std::string& texture_name) const;
	const Animation& getAnimation(const std::string& animation_name) const;
	const sf::Font& getFont(const std::string& font_name) const;
	const sf::Sound& getSound(const std::string& sound_name) const;
};
