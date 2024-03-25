#pragma once

#include "math/Vec2.h"

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


Vec2 getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
Vec2 getPrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

Orientation orientation(const Vec2& a, const Vec2& b, const Vec2& c);

bool isInside(const Vec2& pos, std::shared_ptr<Entity> entity);
bool overlapping(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
bool previouslyOverlapping(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
bool onSegement(const Vec2& a, const Vec2& b, const Vec2& c);
bool lineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);
bool entityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> entity);

} // namespace physics
