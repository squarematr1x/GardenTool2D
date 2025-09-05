#pragma once

#include <tuple>
#include <vector>

#include "../math/vec2.hpp"

class Vec2;
struct Edge;

namespace light
{

struct IntersectPoint {
    float angle;
    Vec2 point;

    IntersectPoint(float angle_in, const Vec2& point_in)
        : angle(angle_in), point(point_in)
    {
    }

    IntersectPoint() = default;
};

std::vector<IntersectPoint> constructVisibilityPoints(const Vec2& source, float r, const std::vector<Edge>& edges);

} // namespace light
