#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "math/Vec2.h"

class Animation
{
	std::string m_name{ "none" };
	sf::Sprite m_sprite;

	size_t m_frame_count{ 1 };
	int m_current_frame{ 0 }; // frame in animation (if animation has 4 "images" frame 2 would be the third image and so on)
	int m_speed{ 0 }; // animation speed

	Vec2 m_size{ 1, 1 }; // (texture_w / frame_count, texture_h)

public:
	Animation() = default;

	Animation(const std::string& name, const sf::Texture& texture);

	Animation(const std::string& name, const sf::Texture& texture, size_t frame_count, int speed);

	void update();

	bool hasEnded();

	const std::string& getName() const { return m_name; }
	const Vec2& getSize() const { return m_size; }
	sf::Sprite& getSprite() { return m_sprite; }
};
