#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "texture-rect.hpp"

#include "../math/vec2.hpp"
#include "../asset/texture.hpp"

class Animation {
	std::string m_name{ "none" };
	TextureRect m_sprite;

	size_t m_frame_count{ 1 };
	int m_current_frame{ 0 }; // frame in animation (if animation has 4 "images" frame 2 would be the third image and so on)
	int m_speed{ 0 }; // animation speed

	Vec2 m_size{ 1, 1 }; // (texture_w/frame_count, texture_h
	Vec2 m_offset{ 0, 0 }; // Starting position inside the texture atlas

public:
	Animation() = default;
	Animation(const std::string& name, const Texture& texture);
	Animation(const std::string& name, const Texture& texture, size_t frame_count, int speed, Vec2 size = Vec2(0, 0), Vec2 offset = Vec2(0, 0));

	void update();

	bool hasEnded();

	const std::string& getName() const { return m_name; }
	const Vec2& getSize() const { return m_size; }
	TextureRect& getTextureRect() { return m_sprite; }
};
