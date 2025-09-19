#include "light.hpp"

#include <algorithm>

#include "edge-pool.hpp"

namespace light
{

std::vector<IntersectPoint> constructVisibilityPoints(const Vec2& source, float r, const std::vector<Edge>& edges) {
    std::vector<IntersectPoint> visibility_points;

    // For each edge in pool
    for (auto& e1 : edges) {
        // Take start point, then endpoint
        // We could use non duplicated points here to optimize
        for (int i = 0; i < 2; i++) {
            float rdx = (i == 0 ? e1.start.x : e1.end.x) - source.x;
            float rdy = (i == 0 ? e1.start.y : e1.end.y) - source.y;

            const float base_ang = atan2f(rdy, rdx);
            float ang = 0.0f;

            // For each point, cast 3 rays, 1 directly at point
            // and one a little bit either side
            for (int j = 0; j < 3; j++) {
                if (j == 0) ang = base_ang - 0.0001f;
                if (j == 1) ang = base_ang;
                if (j == 2) ang = base_ang + 0.0001f;

                // Create ray along angle for required distance
                rdx = r * cosf(ang);
                rdy = r * sinf(ang);

                float min_t1 = std::numeric_limits<float>::infinity();
                float min_px = 0.0f;
                float min_py = 0.0f;
                float min_ang = 0.0f;
                bool valid = false;

                // Check for ray intersection with all edges
                for (auto& e2 : edges) {
                    // Create line segment vector
                    float sdx = e2.end.x - e2.start.x;
                    float sdy = e2.end.y - e2.start.y;

                    if (fabs(sdx - rdx) > 0.0f && fabs(sdy - rdy) > 0.0f) {
                        // t2 is normalized distance from line segment start to line segment end of intersect point
                        float t2 = (rdx * (e2.start.y - source.y) + (rdy * (source.x - e2.start.x))) / (sdx * rdy  - sdy * rdx);
                        // t1 is normalized distance from source along ray to ray length of intersect point
                        float t1 = (e2.start.x + sdx * t2 - source.x) / rdx;

                        // If intersect point exists along ray, and along line
                        // segment thenintersect point is valid
                        if (t1 > 0 && t2 >= 0 && t2 <= 1.0f) {
                            // Check if this intersect point is closest to source
                            if (t1 < min_t1) {
                                min_t1 = t1;
                                min_px = source.x + rdx * t1;
                                min_py = source.y + rdy * t1;
                                min_ang = atan2f(min_py - source.y, min_px - source.x);
                                valid = true;
                            }
                        }
                    }
                }
                if (valid) {
                    visibility_points.push_back({ min_ang, Vec2(min_px, min_py) });
                }
            }
        }
    }
    // Sort by angle from source
    std::sort(
        visibility_points.begin(),
        visibility_points.end(),
        [&](const IntersectPoint &t1, const IntersectPoint &t2) {
            return t1.angle < t2.angle;
        }
    );
    auto it = std::unique(
        visibility_points.begin(),
        visibility_points.end(),
        [&](const IntersectPoint &t1, const IntersectPoint &t2) {
            return fabs(t1.point.x - t2.point.x) < 0.1f && fabs(t1.point.y - t2.point.y) < 0.1f;
        }
    );
    visibility_points.resize(static_cast<size_t>((distance(visibility_points.begin(), it))));

    return visibility_points;
}

} // namespace light
