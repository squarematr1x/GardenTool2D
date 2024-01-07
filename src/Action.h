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
	PLAY,
	PAUSE,
	QUIT,
	TOGGLE_TEXTURE,
	TOGGLE_COLLISION,
	TOGGLE_GRID
};

struct Action
{
	ActionName name{ ActionName::NONE };
	ActionType type{ ActionType::NONE }; // Start or End (enums)

	Action(ActionName name_in, ActionType type_in)
		: name(name_in), type(type_in) // e.g. {"RIGHT", "START"}
	{
	}

	ActionName getName() const { return name; }
	ActionType getType() const { return type; }
};
