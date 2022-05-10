#pragma once

#include <SFML/Graphics.hpp>

#include "Vec2.h"

struct CTransform
{
	Vec2 pos{ 0.0f, 0.0f }; // Center of an object
	Vec2 velocity{ 0.0f, 0.0f };

	float angle{ 0.0f };

	CTransform(const Vec2 p, const Vec2 v, float a)
		: pos(p), velocity(v), angle(a)
	{
	}
};

struct CShape
{
	sf::CircleShape circle;

	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
		: circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);
	}
};

struct CCollision
{
	float radius;

	CCollision(float r)
		: radius(r)
	{
	}
};

struct CScore
{
	int score;

	CScore(int s)
		: score(s)
	{
	}
};

struct CInput
{
	bool up{ false };
	bool left{ false };
	bool right{ false };
	bool down{ false };
	bool shoot{ false };

	CInput()
	{
	}
};

struct CLifespan
{
	int remaining{ 0 };
	int total{ 0 };

	CLifespan(int total)
		: remaining(total), total(total)
	{
	}
};

struct CName
{

};

struct CBBox
{

};
