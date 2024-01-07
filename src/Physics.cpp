#include "Physics.h"
#include "Entity.h"

namespace physics
{
Vec2 getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
	const Vec2 pos_a = a->getComponent<CTransform>().pos;
	const Vec2 pos_b = b->getComponent<CTransform>().pos;
	const Vec2 half_size_a = a->getComponent<CBBox>().half_size;
	const Vec2 half_size_b = b->getComponent<CBBox>().half_size;
	const Vec2 delta = Vec2(
		fabsf(pos_a.x - pos_b.x),
		fabsf(pos_a.y - pos_b.y)
	);

	// Overlap rectangle size of the bounding boxes of entity a and b
	const float overlap_x = half_size_a.x + half_size_b.x - delta.x;
	const float overlap_y = half_size_a.y + half_size_b.y - delta.y;

	return Vec2(overlap_x, overlap_y);
}

Vec2 getPrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
	const Vec2 pos_a = a->getComponent<CTransform>().prevPos;
	const Vec2 pos_b = b->getComponent<CTransform>().prevPos;
	const Vec2 half_size_a = a->getComponent<CBBox>().half_size;
	const Vec2 half_size_b = b->getComponent<CBBox>().half_size;
	const Vec2 delta = Vec2(
		fabsf(pos_a.x - pos_b.x),
		fabsf(pos_a.y - pos_b.y)
	);

	// Overlap rectangle size of the bounding boxes of entity a and b
	const float overlap_x = half_size_a.x + half_size_b.x - delta.x;
	const float overlap_y = half_size_a.y + half_size_b.y - delta.y;

	return Vec2(overlap_x, overlap_y);
}
} // physics
