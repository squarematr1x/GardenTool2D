#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "Vec2.h"

class Animation
{
	std::string m_name{ "" };
	sf::Sprite m_sprite;

	int m_current_frame{ 0 }; // frame in animation (if animation has 4 "images" frame 2 would be the third image and so on)
	int m_frame_count{ 0 };
	int m_speed{ 0 }; // animation speed

	Vec2 m_size; // (texture_w / frame_count, texture_h)

public:
	Animation() = default;

	Animation(const std::string& name, const sf::Texture& texture);

	Animation(const std::string& name, const sf::Texture& texture, int frame_count, int speed);
	void update();

	bool hasEnded();

	const std::string& getName() const { return m_name; }
	const Vec2& getSize() const { return m_size; }
	const sf::Sprite& getSprite() const { return m_sprite; }
};