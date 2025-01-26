#include "assets.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>

#include "../util/profiler.hpp"

void Assets::loadFromFile(const std::string& path) {
	PROFILE_FUNCTION();
	std::ifstream file(path);
	std::string line;

	while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            // Skip comments and empty lines
            continue;
        }

        std::string asset_type;
        std::istringstream text_stream(line);

        while (text_stream >> asset_type) {
			if (asset_type == "Texture") {
				std::string name, texture_path;
				bool repeating;
				text_stream >> name >> texture_path >> repeating;
				addTexture(name, texture_path, repeating);
			} else if (asset_type == "Animation") {
				std::string name, texture;
				size_t frames;
				int speed, w, h, offset_x, offset_y;
				text_stream >> name >> texture >> frames >> speed >> w >> h >> offset_x >> offset_y;
				addAnimation(name, texture, frames, speed, Vec2(w, h), Vec2(offset_x, offset_y));
			} else if (asset_type == "Font") {
				std::string name, font_path;
				text_stream >> name >> font_path;
				addFont(name, font_path);
			} else if (asset_type == "Sound") { 
				std::string name, sound_path;
				text_stream >> name >> sound_path;
				addSound(name, sound_path);
			} else if (asset_type == "Music") { 
				std::string name, music_path;
				text_stream >> name >> music_path;
				addMusic(name, music_path);
			} else if (asset_type == "Layer") {
				std::string name, texture;
				text_stream >> name >> texture;
				addLayer(name, texture);
			} else {
				std::cerr << "Unknown asset type: " << asset_type << '\n';
			}
		}
	}
	file.close();
}

void Assets::addTexture(const std::string& texture_name, const std::string& path, bool repeating, bool smooth) {
	PROFILE_SCOPE(path);
	sf::Texture texture;
	texture.setRepeated(repeating);
	m_texture_map[texture_name] = texture;

	if (!m_texture_map[texture_name].loadFromFile(path)) {
		std::cerr << "Cannot load texture file: " << path << '\n';
		m_texture_map.erase(texture_name);
	} else {
		m_texture_map[texture_name].setSmooth(smooth);
		std::cout << "Loaded Texture: " << path << '\n';
	}
}

void Assets::addAnimation(const std::string& animation_name, const std::string& texture_name, size_t frame_count, int speed, Vec2 size, Vec2 offset) {
	PROFILE_FUNCTION();
	m_animation_map[animation_name] = Animation(animation_name, getTexture(texture_name), frame_count, speed, size, offset);
}

const sf::Texture& Assets::getTexture(const std::string& texture_name) const {
	assert(m_texture_map.find(texture_name) != m_texture_map.end());
	return m_texture_map.at(texture_name);
}

const Animation& Assets::getAnimation(const std::string& animation_name) const {
	assert(m_animation_map.find(animation_name) != m_animation_map.end());
	return m_animation_map.at(animation_name);
}

const std::map<std::string, Animation>& Assets::getAnimations() const {
	return m_animation_map;
}

void Assets::addFont(const std::string& font_name, const std::string& path) {
	PROFILE_FUNCTION();
	m_font_map[font_name] = Font();

	if (!m_font_map[font_name].loadFromFile(path)) {
		std::cerr << "Cannot load font file: " << path << '\n';
		m_font_map.erase(font_name);
	} else {
		std::cout << "Loaded Font: " << path << '\n';
	}
}

const Font& Assets::getFont(const std::string& font_name) const {
	assert(m_font_map.find(font_name) != m_font_map.end());
	return m_font_map.at(font_name);
}

void Assets::addSoundBuffer(const std::string& sound_name, const std::string& path) {
	PROFILE_FUNCTION();
	m_sound_buffer_map[sound_name] = sf::SoundBuffer();

	if (!m_sound_buffer_map[sound_name].loadFromFile(path)) {
		std::cerr << "Cannot load sound file: " << path << '\n';
		m_font_map.erase(sound_name);
	} else {
		std::cout << "Loaded Sound: " << path << '\n';
	}
}

const sf::SoundBuffer& Assets::getSoundBuffer(const std::string& sound_name) const {
	assert(m_sound_buffer_map.find(sound_name) != m_sound_buffer_map.end());
	return m_sound_buffer_map.at(sound_name);
}

void Assets::addSound(const std::string& sound_name, const std::string& path) {
	PROFILE_FUNCTION();
	addSoundBuffer(sound_name, path);
	m_sound_map[sound_name] = sf::Sound(getSoundBuffer(sound_name));
}

sf::Sound Assets::getSound(const std::string& sound_name) const {
	assert(m_sound_map.find(sound_name) != m_sound_map.end());
	return m_sound_map.at(sound_name);
}

void Assets::addMusic(const std::string& music_name, const std::string& path) {
	PROFILE_FUNCTION();
	m_music_map[music_name] = std::make_shared<sf::Music>();
	if (!m_music_map[music_name]->openFromFile(path)) {
		std::cerr << "Cannot load music file: " << path << '\n';
		m_music_map.erase(music_name);
	} else {
		std::cout << "Loaded Music: " << path << '\n';
	}
}

const std::shared_ptr<sf::Music> Assets::getMusic(const std::string& music_name) const {
	assert(m_music_map.find(music_name) != m_music_map.end());
	return m_music_map.at(music_name);
}


void Assets::addLayer(const std::string& layer_name, const std::string& texture_name) {
	m_layer_map[layer_name] = Layer(layer_name, getTexture(texture_name));
}

const Layer& Assets::getLayer(const std::string& layer_name) const {
	assert(m_layer_map.find(layer_name) != m_layer_map.end());
	return m_layer_map.at(layer_name);
}
