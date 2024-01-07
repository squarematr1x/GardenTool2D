#pragma once

#include "Vec2.h"

class Entity;

namespace physics
{

	Vec2 getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	Vec2 getPrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

} // namespace physics
