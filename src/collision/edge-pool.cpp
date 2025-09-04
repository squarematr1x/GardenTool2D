#include "edge-pool.hpp"

#include "../ecs/entity-manager.hpp"
#include "../engine.hpp"

void EdgePool::constructPool(EntityManager& entity_manager, GameEngine* engine) {
    std::unordered_set<Vec2, Vec2Hasher> vertices;
    std::vector<Cell> grid;
    size_t w_steps = 0;
    auto boundary = getWorldBoundary(entity_manager, engine, vertices);

    // Init grid
    for (float y = boundary.min.y; y <= boundary.max.y; y += 64.0f) {
        w_steps = 0;
        for (float x = boundary.min.x; x <= boundary.max.x; x+= 64.0f) {
            Cell cell;
            if (auto search = vertices.find(Vec2(x, y)); search != vertices.end()) {
                cell.exists = true;
            }
            grid.push_back(cell);
            w_steps++;
        }
    }

    if (grid.size() <= 0) {
        return;
    }

    size_t i = 0;
    for (float y = boundary.min.y; y <= boundary.max.y; y += 64.0f) {
        for (float x = boundary.min.x; x <= boundary.max.x; x+= 64.0f) {
            if (!grid[i].exists) {
                i++;
                continue;
            }

            const size_t w = i - 1;
            const size_t e = i + 1;
            const size_t n = i - w_steps;
            const size_t s = i + w_steps;

            // If this cell has no western neighbour it needs western edge
            if (!validIndex(w, grid) || (validIndex(w, grid) && !grid[w].exists)) {
                if (validIndex(n, grid) && grid[n].edge_exists[WEST]) {
                    // Norther neighbour has a western edge, so grow it downwards
                    m_edges[grid[n].edge_id[WEST]].end.y += 68.0f;
                    grid[i].edge_exists[WEST] = true;
                    grid[i].edge_id[WEST] = grid[n].edge_id[WEST];
                } else {
                    // Create new edge
                    grid[i].edge_id[WEST] = m_edges.size();
                    grid[i].edge_exists[WEST] = true;
                    m_edges.push_back({
                        { x - 32.0f, y - 32.0f },
                        { x - 32.0f, y + 32.0f }
                    });
                }
            }

            // If this cell has no eastern neighbour it needs eastern edge
            if (!validIndex(e, grid) || (validIndex(e, grid) && !grid[e].exists)) {
                if (validIndex(n, grid) && grid[n].edge_exists[EAST]) {
                    // Norther neighbour has a eastern edge, so grow it downwards
                    m_edges[grid[n].edge_id[EAST]].end.y += 64.0f;
                    grid[i].edge_exists[EAST] = true;
                    grid[i].edge_id[EAST] = grid[n].edge_id[EAST];
                } else {
                    // Create new edge
                    grid[i].edge_id[EAST] = m_edges.size();
                    grid[i].edge_exists[EAST] = true;
                    m_edges.push_back({
                        { x + 32.0f, y - 32.0f },
                        { x + 32.0f, y + 32.0f }
                    });
                }
            }

            // If this cell has no northern neighbour it needs northern edge
            if (!validIndex(n, grid) || (validIndex(n, grid) && !grid[n].exists)) {
                if (validIndex(w, grid) && grid[w].edge_exists[NORTH]) {
                    // Western neighbour has a northern edge, so grow it eastwards
                    m_edges[grid[w].edge_id[NORTH]].end.x += 64.0f;
                    grid[i].edge_exists[NORTH] = true;
                    grid[i].edge_id[NORTH] = grid[w].edge_id[NORTH];
                } else {
                    // Create new edge
                    grid[i].edge_id[NORTH] = m_edges.size();
                    grid[i].edge_exists[NORTH] = true;
                    m_edges.push_back({
                        { x - 32.0f, y - 32.0f },
                        { x + 32.0f, y - 32.0f }
                    });
                }
            }

            // If this cell has no southern neighbour it needs southern edge
            if (!validIndex(s, grid) || (validIndex(s, grid) && !grid[s].exists)) {
                if (validIndex(w, grid) && grid[w].edge_exists[SOUTH]) {
                    // Western neighbour has a southern edge, so grow it eastwards
                    m_edges[grid[w].edge_id[SOUTH]].end.x += 64.0f;
                    grid[i].edge_exists[SOUTH] = true;
                    grid[i].edge_id[SOUTH] = grid[w].edge_id[SOUTH];
                } else {
                    // Create new edge
                    grid[i].edge_id[SOUTH] = m_edges.size();
                    grid[i].edge_exists[SOUTH] = true;
                    m_edges.push_back({
                        { x - 32.0f, y + 32.0f },
                        { x + 32.0f, y + 32.0f }
                    });
                }
            }
            i++;
        }
    }
}

const Boundary EdgePool::getWorldBoundary(EntityManager& entity_manager, GameEngine* engine, std::unordered_set<Vec2, Vec2Hasher>& vertices) {
    Vec2 min(0.0f, 0.0f);
    Vec2 max(engine->window().getSize().x, engine->window().getSize().y);

    for (auto& e : entity_manager.getEntities(Tag::TILE)) {
        if (!e->hasComponent<CBBox>()) {
            continue;
        }

        const auto transform = e->getComponent<CTransform>();

        if (transform.pos.x < min.x) {
            min.x = transform.pos.x;
        }
        if (transform.pos.x > max.x) {
            max.x = transform.pos.x;
        }
        if (transform.pos.y < min.y) {
            min.y = transform.pos.y;
        }
        if (transform.pos.y > max.y) {
            max.y = transform.pos.y;
        }

        vertices.insert(transform.pos);
    }

    return Boundary{ min, max };
}

bool EdgePool::validIndex(size_t index, std::vector<Cell> cells) {
    return cells.size() > 0 && index >= 0 && index < cells.size();
}
