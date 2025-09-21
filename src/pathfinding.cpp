#include "pathfinding.hpp"

#include <queue>
#include <unordered_set>

#include "ecs/entity-manager.hpp"
#include "collision/physics.hpp"

namespace path
{

// getPath uses weighted a* algorithm
std::vector<Vec2> getPath(const Vec2& start, const Vec2& goal, EntityManager& entity_manager) {
    if (!validGoal(goal, entity_manager)) {
        return {};
    }

    std::unordered_set<Vec2, Vec2Hasher> closed;
    std::priority_queue<SearchNode, std::vector<SearchNode>, std::greater<SearchNode>> open;
    std::map<Vec2, SearchNode> open_map;
    std::vector<Vec2> path;

    unsigned int step = 0;
    constexpr unsigned int max_steps = 100;

    constexpr auto g = 0.0f;
    const auto h = start.distance(goal, Hearistic::EUCLIDIC);
    constexpr auto w = 1000.0f;
    const auto f = g + w*h;

    const SearchNode start_node(start, f, g);

    open.push(start_node);
    open_map.insert({start_node.pos, start_node});

    while (!open.empty()) {
        auto current = open.top();

        if (current.pos == goal) {
            while (true) {
                current = open_map.at(current.parent);
                if (current.root) {
                    break;
                }
                path.push_back(current.pos);
            }
            std::reverse(path.begin(), path.end());
            path.push_back(goal);
            return path;
        }

        open.pop();
        if (auto search = closed.find(current.pos); search != closed.end()) {
            continue;
        }
        closed.insert(current.pos);

        const auto neighbours = getNeighbours(current, goal, entity_manager);
        for (const auto& neighbour : neighbours) {
            if (auto search = closed.find(neighbour.pos); search != closed.end()) {
                continue;
            }
            if (open_map.find(neighbour.pos) != open_map.end()) {
                const auto prev_g = open_map.at(neighbour.pos);
                if (prev_g.g <= neighbour.g) {
                    continue;
                }
            }
            open.push(neighbour);
            open_map.insert({neighbour.pos, neighbour});
        }

        if (++step > max_steps) {
            // Player probably clicked inside a closed area (no path can be found)
            return {};
        }
    }

    return path;
}

std::vector<SearchNode> getNeighbours(const SearchNode& node, const Vec2& goal, EntityManager& entity_manager, const Vec2& cell_size) {
    // Get these four neighbouring positions from the grid:
    // [ ] [1] [ ]
    // [4] pos [2] 
    // [ ] [3] [ ]

    const auto parent_pos = node.pos;
    std::vector<Vec2> positions;

    // 1:
    positions.emplace_back(parent_pos.x, parent_pos.y - cell_size.y);
    // 2:
    positions.emplace_back(parent_pos.x + cell_size.x, parent_pos.y);
    // 3:
    positions.emplace_back(parent_pos.x, parent_pos.y + cell_size.y);
    // 4:
    positions.emplace_back(parent_pos.x - cell_size.y, parent_pos.y);

    std::vector<SearchNode> nodes;
    constexpr auto w = 1000.0f;
    const auto tiles = entity_manager.getEntities(Tag::TILE);
    for (const auto& pos : positions) {
        bool collide = false;
        for (auto tile : tiles) {
            if (tile.hasComponent<CBBox>() && physics::isInside(pos, tile)) {
                collide = true;
                break;
            }
        }
        if (!collide) {
            auto g = node.g + cell_size.x;
            auto h = pos.distance(goal, Hearistic::EUCLIDIC);
            auto f = g + w*h;
            nodes.push_back({ pos, parent_pos, f, g });
        }
    }

    return nodes;
}

bool validGoal(const Vec2& goal, EntityManager& entity_manager) {
    const auto tiles = entity_manager.getEntities(Tag::TILE);
    for (auto tile : tiles) {
        if (tile.hasComponent<CBBox>() && physics::isInside(goal, tile)) {
            return false;
        }
    }
    return true;
}

} // namespace path
