#pragma once

#include <string>

struct Action
{
	std::string name{ "NONE" };
	std::string type{ "NONE" }; // Start or End (enums)

	Action(std::string name_in, std::string type_in)
		: name(name_in), type(type_in) // e.g. {"RIGHT", "START"}
	{
	}

	std::string getName() const { return name; }
	std::string getType() const { return type; }
};
