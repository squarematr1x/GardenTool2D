#include "physics.hpp"
#include "../ecs/entity.hpp"

namespace physics
{

Vec2 getOverlap(Entity a, Entity b) {
	const auto pos_a = a.getComponent<CTransform>().pos;
	const auto pos_b = b.getComponent<CTransform>().pos;
	const auto half_size_a = a.getComponent<CBBox>().half_size;
	const auto half_size_b = b.getComponent<CBBox>().half_size;
	const auto delta = Vec2(
		fabsf(pos_a.x - pos_b.x),
		fabsf(pos_a.y - pos_b.y)
	);

	// Overlap rectangle size of the bounding boxes of entity a and b
	const auto overlap_x = half_size_a.x + half_size_b.x - delta.x;
	const auto overlap_y = half_size_a.y + half_size_b.y - delta.y;

	return Vec2(overlap_x, overlap_y);
}

Vec2 getPrevOverlap(Entity a, Entity b) {
	const auto pos_a = a.getComponent<CTransform>().prev_pos;
	const auto pos_b = b.getComponent<CTransform>().prev_pos;
	const auto half_size_a = a.getComponent<CBBox>().half_size;
	const auto half_size_b = b.getComponent<CBBox>().half_size;
	const auto delta = Vec2(
		fabsf(pos_a.x - pos_b.x),
		fabsf(pos_a.y - pos_b.y)
	);

	// Overlap rectangle size of the bounding boxes of entity a and b
	const auto overlap_x = half_size_a.x + half_size_b.x - delta.x;
	const auto overlap_y = half_size_a.y + half_size_b.y - delta.y;

	return Vec2(overlap_x, overlap_y);
}

bool overlapping(Entity a, Entity b) {
	const auto overlap = getOverlap(a, b);
	
	return (overlap.x > 0 && overlap.y > 0);
}

bool previouslyOverlapping(Entity a, Entity b) {
	const auto prev_overlap = getPrevOverlap(a, b);
	
	return (prev_overlap.x > 0 && prev_overlap.y > 0);
}

bool isInside(const Vec2& pos, Entity entity) {
	const auto e_pos = entity.getComponent<CTransform>().pos;
    auto size = Vec2(0, 0); 

	if (entity.hasComponent<CAnimation>()) {
		size = entity.getComponent<CAnimation>().animation.getSize();
	}
	if (entity.hasComponent<CBBox>()) {
		size = entity.getComponent<CBBox>().size;
	}

    const auto dx = fabs(pos.x - e_pos.x);
    const auto dy = fabs(pos.y - e_pos.y);

    return (dx <= size.x/2) && (dy <= size.y/2);
}

Orientation orientation(const Vec2& a, const Vec2& b, const Vec2& c) {
	const auto val = (b.y - a.y)*(c.x - b.x) - (b.x - a.x)*(c.y - b.y);

	if (val == 0.0f) {
		return Orientation::COLLINEAR;
	}
	if (val > 0.0f) {
		return Orientation::CLOCKWISE;
	}

	return Orientation::COUNTERCLOCKWISE;
}

bool onSegement(const Vec2& a, const Vec2& b, const Vec2& c) {
	return (
		b.x <= std::max(a.x, c.x) && b.x >= std::min(a.x, c.x) &&
		b.y <= std::max(a.y, c.y) && b.y >= std::min(a.y, c.y)
	);
}

bool lineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d) {
	const auto o1 = orientation(a, b, c);
	const auto o2 = orientation(a, b, d);
	const auto o3 = orientation(c, d, a);
	const auto o4 = orientation(c, d, b);

	if (o1 != o2 && o3 != o4) {
		return true;
	}

	if (o1 == Orientation::COLLINEAR && onSegement(a, c, b)) {
		return true;
	}
	if (o2 == Orientation::COLLINEAR && onSegement(a, d, b)) {
		return true;
	}
	if (o3 == Orientation::COLLINEAR && onSegement(c, a, d)) {
		return true;
	}
	if (o4 == Orientation::COLLINEAR && onSegement(c, b, d)) {
		return true;
	}

	return false;
}

bool entityIntersect(const Vec2& a, const Vec2& b, Entity entity) {
	const auto half_size = entity.getComponent<CBBox>().half_size;
	const auto pos = entity.getComponent<CTransform>().pos;
	const Vec2 p1(pos.x - half_size.x, pos.y - half_size.y);
	const Vec2 p2(pos.x + half_size.x, pos.y - half_size.y);
	const Vec2 p3(pos.x + half_size.x, pos.y + half_size.y);
	const Vec2 p4(pos.x - half_size.x, pos.y + half_size.y);

	if (lineIntersect(a, b, p1, p2)) {
		return true;
	}
	if (lineIntersect(a, b, p2, p3)) {
		return true;
	}
	if (lineIntersect(a, b, p3, p4)) {
		return true;
	}
	if (lineIntersect(a, b, p1, p4)) {
		return true;
	}

	return false;
}

} // namespace physics
