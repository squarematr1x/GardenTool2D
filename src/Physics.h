#pragma once

#include "Vec2.h"

class Entity;

namespace physics
{
	struct Intersect {
		bool isIntersecting{ false };
		Vec2 pos{ 0, 0 };
	};


	Vec2 getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	Vec2 getPrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

	bool isInside(const Vec2& pos, std::shared_ptr<Entity> entity);
	bool entityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> entity);

	Intersect lineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);

} // namespace physics
