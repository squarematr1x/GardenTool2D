#pragma once

#include "Vec2.h"

#include "Animation.h"

struct Component {
	bool has{ false };
};

struct CTransform: Component
{
	Vec2 pos{ 0.0f, 0.0f }; // Center of an object
	Vec2 velocity{ 0.0f, 0.0f };
	Vec2 prevPos{ 0.0f, 0.0f };
	Vec2 scale{ 0.0f, 0.0f };

	float angle{ 0.0f };

	CTransform(const Vec2 p, const Vec2 v, float a)
		: pos(p), velocity(v), angle(a)
	{
	}
};

struct CShape: Component
{
	sf::CircleShape circle;

	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
		: circle(radius, static_cast<std::size_t>(points))
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);
	}
};

struct CCollision: Component
{
	float radius;

	CCollision(float r)
		: radius(r)
	{
	}
};

struct CScore: Component
{
	int score;

	CScore(int s)
		: score(s)
	{
	}
};

struct CInput: Component
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

struct CLifespan: Component
{
	int remaining{ 0 };
	int total{ 0 };

	CLifespan(int total)
		: remaining(total), total(total)
	{
	}
};

struct CBBox: Component
{
	Vec2 size;
	Vec2 half_size; // To reduce computations

	CBBox(Vec2 size_in)
		: size(size_in), half_size(size_in / 2)
	{
	}
};

struct CAnimation: Component
{
	Animation animation;
	bool repeat{ false };
	
	CAnimation() {}
	CAnimation(const Animation& animation_in, bool repeat_in)
		: animation(animation_in), repeat(repeat_in)
	{
	}
};

struct CGravity: Component
{
	float gravity{ 0 };

	CGravity() {}
	CGravity(float gravity_in)
		: gravity(gravity_in)
	{
	}
};

struct CState: Component
{
	std::string state{ "jump" };

	CState() {}
	CState(const std::string& state_in)
		: state(state_in)
	{
	}
};