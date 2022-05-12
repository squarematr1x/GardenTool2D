#include "Assets.h"

#include <fstream>
#include <iostream>
#include <cassert>

Assets::Assets()
{
}

void Assets::loadFromFile(const std::string& path)
{
	std::ifstream file(path);
	std::string str;

	while (file.good())
	{
		file >> str;

		if (str == "Texture")
		{
			std::string name, texture_path;
			file >> name >> texture_path;
			addTexture(name, texture_path);
		}
		else if (str == "Animation")
		{
			std::string name, texture;
			int frames, speed;
			file >> name >> texture >> frames >> speed;
			addAnimation(name, texture, frames, speed);
		}
		else if (str == "Font")
		{
			std::string name, font_path;
			file >> name >> font_path;
			addFont(name, font_path);
		}
		else
		{
			std::cerr << "Unknown asset type: " << str << '\n';
		}
	}
}

void Assets::addTexture(const std::string& texture_name, const std::string& path, bool smooth)
{
	m_texture_map[texture_name] = sf::Texture();

	if (!m_texture_map[texture_name].loadFromFile(path))
	{
		std::cerr << "Cannot load texture file: " << path << '\n';
		m_texture_map.erase(texture_name);
	}
	else
	{
		m_texture_map[texture_name].setSmooth(smooth);
		std::cout << "Loaded Texture: " << path << '\n';
	}
}

void Assets::addAnimation(const std::string& animation_name, const std::string& texture_name, int frame_count, int speed)
{
	m_animation_map[animation_name] = Animation(animation_name, getTexture(texture_name), frame_count, speed);
}

const sf::Texture& Assets::getTexture(const std::string& texture_name) const
{
	assert(m_texture_map.find(texture_name) != m_texture_map.end());
	return m_texture_map.at(texture_name);
}

const Animation& Assets::getAnimation(const std::string& animation_name) const
{
	assert(m_animation_map.find(animation_name) != m_animation_map.end());
	return m_animation_map.at(animation_name);
}

void Assets::addFont(const std::string& font_name, const std::string& path)
{
	m_font_map[font_name] = sf::Font();

	if (!m_font_map[font_name].loadFromFile(path))
	{
		std::cerr << "Cannot load font file: " << path << '\n';
		m_font_map.erase(font_name);
	}
	else
	{
		std::cout << "Loaded Font: " << path << '\n';
	}
}

const sf::Font& Assets::getFont(const std::string& font_name) const
{
	assert(m_font_map.find(font_name) != m_font_map.end());
	return m_font_map.at(font_name);
}
