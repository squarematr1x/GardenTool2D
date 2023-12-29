#pragma once

#include <string>

struct Action
{
	std::string name;
	std::string type; // Start or End (enums)

	Action(std::string name_in, std::string type_in)
		: name(name_in), type(type_in) // e.g. {"right", "start"}
	{
	}

	std::string getName() const { return name; }
	std::string getType() const { return type; }
};
