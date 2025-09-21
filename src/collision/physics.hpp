#pragma once

#include "../math/vec2.hpp"

class Entity;

namespace physics
{

enum class Orientation : unsigned char {
	COLLINEAR,
	CLOCKWISE,
	COUNTERCLOCKWISE
};

struct Intersect { // TODO: Use this later when needed
	bool isIntersecting{ false };
	Vec2 pos{ 0, 0 };
};


Vec2 getOverlap(Entity a, Entity b);
Vec2 getPrevOverlap(Entity a, Entity b);

Orientation orientation(const Vec2& a, const Vec2& b, const Vec2& c);

bool isInside(const Vec2& pos, Entity entity);
bool overlapping(Entity a, Entity b);
bool previouslyOverlapping(Entity a, Entity b);
bool onSegement(const Vec2& a, const Vec2& b, const Vec2& c);
bool lineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);
bool entityIntersect(const Vec2& a, const Vec2& b, Entity entity);

} // namespace physics
