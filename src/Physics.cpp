#include "Physics.h"
#include "Components.h"

Vec2 getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// auto& pos = a->getComponent<CTransform>().pos;
	(void)a;
	(void)b;
	// return the overlap rectangle size of the bounding boxes of entity a and b
	return Vec2(0, 0);
}

Vec2 getPrevOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	(void)a;
	(void)b;
	// return the previous overlap (uses entity's previous position)
	return Vec2(0, 0);
}
