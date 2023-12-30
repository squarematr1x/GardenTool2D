#pragma once

#include "Vec2.h"

class Entity;

namespace Physics
{
	Vec2 getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	Vec2 getPrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

	// bool isCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	// bool isIntersect(line, line);
	// bool isInside(Vec2, line);
} // Physics
