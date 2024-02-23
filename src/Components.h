#pragma once

#include "Vec2.h"

#include "Animation.h"

enum class State: unsigned char {
	NONE,
	// Side scroller
	JUMP,
	STAND,
	RUN,
	// Top down RPG
	UP,
	DOWN,
	RIGHT,
	LEFT,
	ATTACK_UP,
	ATTACK_DOWN,
	ATTACK_RIGHT,
	ATTACK_LEFT
};

struct Component {
	bool has{ false };
};

struct CTransform: Component {
	Vec2 pos{ 0.0f, 0.0f }; // Center of an object
	Vec2 velocity{ 0.0f, 0.0f };
	Vec2 prev_pos{ 0.0f, 0.0f };
	Vec2 scale{ 1.0f, 1.0f };
	Vec2 facing{ 0.0f, 1.0f }; // TODO: I guess I missed this!?

	float angle{ 0.0f };

	CTransform() {}
	CTransform(const Vec2& p) 
		: pos(p)
	{
	}

	CTransform(const Vec2& p, const Vec2& v) 
		: pos(p), velocity(v) 
	{
	}

	CTransform(const Vec2& p, const Vec2& v, float a)
		: pos(p), velocity(v), angle(a)
	{
	}
};

struct CShape: Component {
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

struct CCollision: Component {
	float radius;

	CCollision(float r)
		: radius(r)
	{
	}
};

struct CScore: Component {
	int score;

	CScore(int s)
		: score(s)
	{
	}
};

struct CHealth: Component {
	int max{ 4 };
	int current{ 4 };
	float percentage{ 1.0f }; // Store for rendering purposes

	CHealth() {}
	CHealth(int max_in)
		: max(max_in), current(max_in)
	{
	} 

	CHealth(int max_in, int current_in)
		: max(max_in),
		current(current_in),
		percentage(static_cast<float>(current_in) / static_cast<float>(max_in))
	{
	} 
};

struct CInput: Component {
	bool up{ false };
	bool left{ false };
	bool right{ false };
	bool down{ false };
	bool attack{ false };

	CInput() {}
};

struct CLifespan: Component {
	int remaining{ 0 }; // TODO: change to life_span
	int total{ 0 }; // TODO: change to frame_created

	CLifespan() {}
	CLifespan(int total)
		: remaining(total), total(total)
	{
	}
};

struct CDamage: Component {
	int damage{ 1 };

	CDamage() {}
	CDamage(int damage_in)
		: damage(damage_in)
	{
	}
};

struct CInvincibility: Component {
	int i_frames{ 60 };

	CInvincibility() {}
	CInvincibility(int i_frames_in)
		: i_frames(i_frames_in)
	{
	}
};

struct CBBox: Component {
	Vec2 size{ 0.0f, 0.0f };
	Vec2 half_size{ 0.0f, 0.0f }; // To reduce computations
	bool block_movement{ false };
	bool block_vision{ false };

	CBBox() {}
	CBBox(Vec2 size_in)
		: size(size_in), half_size(size_in / 2)
	{
	}

	CBBox(Vec2 size_in, bool block_movement_in, bool block_vision_in)
		: size(size_in),
		half_size(size_in / 2),
		block_movement(block_movement_in),
		block_vision(block_vision_in)
	{
	}
};

struct CAnimation: Component {
	Animation animation;
	bool repeat{ false };
	
	CAnimation() {}
	CAnimation(const Animation& animation_in)
		: animation(animation_in)
	{
	}

	CAnimation(const Animation& animation_in, bool repeat_in)
		: animation(animation_in), repeat(repeat_in)
	{
	}
};

struct CGravity: Component {
	float gravity{ 0 };

	CGravity() {}
	CGravity(float gravity_in)
		: gravity(gravity_in)
	{
	}
};

struct CState: Component {
	State state{ State::NONE };
	State prev_state{ State::NONE };

	CState() {}
	CState(State state_in)
		: state(state_in)
	{
	}
};

struct CDraggable: Component {
	bool dragged{ false };

	CDraggable() {}
};

struct CFollowPlayer: Component {
	Vec2 home{ 0, 0 };
	float speed{ 0.0f };
	bool detected{ false };

	CFollowPlayer() {}
	CFollowPlayer(Vec2 home_in, float velocity_in)
		: home(home_in), speed(velocity_in) 
	{
	}
};

struct CPatrol: Component {
	std::vector<Vec2> positions;
	size_t cur_pos{ 0 };
	float speed{ 0.0f };

	CPatrol() {}
	CPatrol(const std::vector<Vec2>& positions_in, float speed_in)
		: positions(positions_in), speed(speed_in)
	{
	}
};
