#pragma once

#include <string>

enum class ActionType : unsigned char {
	NONE,
	START,
	END
};

enum class ActionName : unsigned char {
	NONE,
	UP,
	RIGHT,
	DOWN,
	LEFT,
	SHOOT,
	ATTACK,
	PLAY,
	PAUSE,
	QUIT,
	TOGGLE_TEXTURE,
	TOGGLE_COLLISION,
	TOGGLE_GRID,
	TOGGLE_LEVEL_EDITOR,
	TOGGLE_FOLLOW,
	TOGGLE_HEALTH,
	TOGGLE_AI_INFO,
	TOGGLE_LIGHT,
	TOGGLE_NOISE_AREA,
	LEFT_CLICK,
	MIDDLE_CLICK,
	RIGHT_CLICK,
	MOUSE_MOVE,
	MOUSE_SCROLL,
	L_SYSTEM
};

struct Action {
	ActionName name{ ActionName::NONE };
	ActionType type{ ActionType::NONE };
	Vec2 pos{ Vec2(0, 0) };
	float delta{ 1.0f };

	Action(ActionName name_in, ActionType type_in)
		: name(name_in), type(type_in)
	{
	}

	Action(ActionName name_in, ActionType type_in, const Vec2& pos_in)
		: name(name_in), type(type_in), pos(pos_in)
	{
	}

	Action(ActionName name_in, ActionType type_in, float delta_in)
		: name(name_in), type(type_in), delta(delta_in)
	{
	}

	ActionName getName() const { return name; }
	ActionType getType() const { return type; }
	const Vec2& getPos() const { return pos; }
};
