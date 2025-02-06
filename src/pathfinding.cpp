#include "pathfinding.hpp"

#include <queue>

#include "ecs/entity-manager.hpp"
#include "physics.hpp"

namespace path
{

std::vector<Vec2> getPath(const Vec2& start, const Vec2& goal, EntityManager& entity_manager) {
    // This function uses A* algorithm

    std::vector<Vec2> closed;
    std::priority_queue<SearchNode, std::vector<SearchNode>, CompareNode> open;

    constexpr auto g = 0.0f;
    const auto h = start.distance(goal);
    constexpr auto w = 10.0f;
    const auto f = g + w*h;

    SearchNode start_node(start, f, g);

    open.push(start_node);

    while (!open.empty()) {
        auto current = open.top();

        if (current.pos == goal) {
            closed.push_back(goal);
            break;
        }

        open.pop();
        if (contains(closed, current.pos)) {
            continue;
        }
        closed.push_back(current.pos);

        auto neighbours = getNeighbours(current, goal, entity_manager);
        for (const auto& neighbour : neighbours) {
            if (contains(closed, neighbour.pos)) {
                continue;
            }
            open.push(neighbour);
        }
    }

    return closed;
}

std::vector<SearchNode> getNeighbours(const SearchNode& node, const Vec2& goal, EntityManager& entity_manager, const Vec2& cell_size) {
    // Get these four neighbouring positions from the grid:
    // [ ] [1] [ ]
    // [4] pos [2] 
    // [ ] [3] [ ]

    const auto pos = node.pos;
    std::vector<Vec2> positions;

    // 1:
    positions.emplace_back(pos.x, pos.y - cell_size.y);
    // 2:
    positions.emplace_back(pos.x + cell_size.x, pos.y);
    // 3:
    positions.emplace_back(pos.x, pos.y + cell_size.y);
    // 4:
    positions.emplace_back(pos.x - cell_size.y, pos.y);

    std::vector<SearchNode> nodes;
    constexpr auto w = 10.0f;
    auto tiles = entity_manager.getEntities(Tag::TILE);
    for (const auto& pos : positions) {
        bool collide = false;
        for (auto tile : tiles) {
            if (tile->hasComponent<CBBox>() && physics::isInside(pos, tile)) {
                collide = true;
            }
        }
        if (!collide) {
            auto g = node.g + cell_size.x;
            auto h = pos.distance(goal);
            auto f = g + w*h;
            nodes.push_back({ pos, f, g });
        }
    }

    return nodes;
}

bool contains(const std::vector<Vec2> v, const Vec2& p) {
    if (v.empty()) {
        return false;
    }
    return (std::find(v.begin(), v.end(), p) != v.end());
}

} // namespace path


