#pragma once

#include <tuple>
#include <vector>

class Vec2;
struct Edge;

namespace light
{

std::vector<std::tuple<float, float, float>> constructVisibilityPoints(const Vec2& source, float r, const std::vector<Edge>& edges);

} // namespace light
