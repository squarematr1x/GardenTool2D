#pragma once

#include <map>
#include <string>
#include <memory>

#include "animation.hpp"
#include "font.hpp"
#include "texture.hpp"
#include "sound.hpp"
#include "music.hpp"
#include "../layer.hpp"
#include "../math/vec2.hpp"

class Assets
{
	// TODO: Prefer enums over strings?
	std::map<std::string, Texture> m_texture_map;
	std::map<std::string, Animation> m_animation_map;
	std::map<std::string, Font> m_font_map;
	std::map<std::string, Sound> m_sound_map;
	std::map<std::string, std::shared_ptr<Music>> m_music_map;
	std::map<std::string, Layer> m_layer_map;

	void addTexture(const std::string& texture_name, const std::string& path, bool repeating, bool smooth = false);
	void addAnimation(const std::string& animation_name, const std::string& texture_name, size_t frame_count, int speed, Vec2 size, Vec2 offset);
	void addFont(const std::string& font_name, const std::string& path);
	void addSound(const std::string& sound_name, const std::string& path);
	void addMusic(const std::string& music_name, const std::string& path);
	void addLayer(const std::string& layer_name, const std::string& texture_name);

public:
	void loadFromFile(const std::string& path);

	const Texture& getTexture(const std::string& texture_name) const;
	const Animation& getAnimation(const std::string& animation_name) const;
	const std::map<std::string, Animation>& getAnimations() const;
	const Font& getFont(const std::string& font_name) const;
	Sound getSound(const std::string& sound_name) const;
	const std::shared_ptr<Music> getMusic(const std::string& music_name) const;
	const Layer& getLayer(const std::string& layer_name) const;
};
