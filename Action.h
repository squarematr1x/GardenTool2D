#pragma once

#include <string>

struct Action
{
	std::string name;
	std::string type;

	Action(std::string name_in, std::string type_in)
		: name(name_in), type(type_in)
	{
	}

	std::string name() const { return name; }
	std::string type() const { return type; }
};
