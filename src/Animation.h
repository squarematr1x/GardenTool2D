#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "Vec2.h"

class Animation
{
	std::string m_name{ "" };
	sf::Sprite m_sprite;

	int m_current_frame{ 0 };
	int m_speed{ 0 };

	Vec2 m_size;

public:
	Animation() = default;

	Animation(const std::string& name, sf::Texture texture, int frame_count, int speed)
		: m_name(name), m_sprite(texture), m_current_frame(frame_count), m_speed(speed)
	{
	}

	void update();

	bool hasEnded();

	const std::string& getName() const { return m_name; }
	const Vec2& getSize() const { return m_size; }
	const sf::Sprite& getSprite() const { return m_sprite; }
};
