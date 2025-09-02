#include "edge-pool.hpp"

#include <unordered_set>

#include "../ecs/entity-manager.hpp"
#include "../engine.hpp"

void EdgePool::constructPool(EntityManager& entity_manager, GameEngine* engine) {
    std::unordered_set<Vec2, Vec2Hasher> vertices;

    for (auto& e : entity_manager.getEntities(Tag::TILE)) {
        if (!e->hasComponent<CBBox>()) {
            continue;
        }
        vertices.insert(e->getComponent<CTransform>().pos);
    }

    auto boundaries = getWorldBoundary(entity_manager, engine);
    std::vector<Cell> cells;
    size_t w_steps = 0;

    // Init
    for (float y = boundaries.min.y; y <= boundaries.max.y; y += 64.0f) {
        w_steps = 0;
        for (float x = boundaries.min.x; x <= boundaries.max.x; x+= 64.0f) {
            Cell cell;
            if (auto search = vertices.find(Vec2(x, y)); search != vertices.end()) {
                cell.exists = true;
            }
            cells.push_back(cell);
            w_steps++;
        }
    }

    if (cells.size() <= 0) {
        return;
    }

    size_t i = 0;
    for (float x = boundaries.min.x; x <= boundaries.max.x; x += 64.0f) {
        for (float y = boundaries.min.y; y <= boundaries.max.y; y+= 64.0f) {
            auto cell = cells[i];
            const size_t w = i - 1;
            const size_t e = i + 1;
            const size_t n = i - w_steps;
            const size_t s = i + w_steps;

            if (cells[i].exists) {
                // If this cell has no western neighbour it needs western edge
                if (validIndex(w, cells) && !cells[w].exists) {
                    if (validIndex(n, cells) && cells[n].edge_exists[WEST]) {
                        // Norther neighbour has a western edge, so grow it downwards
                        m_edges[cells[n].edge_id[WEST]].end.y += 64.0f;
                        cell.edge_exists[WEST] = true;
                        cell.edge_id[WEST] = cells[n].edge_id[WEST];
                    } else {
                        // Create new edge
                        auto edge_id = m_edges.size();
                        m_edges.push_back({
                            { x - 32.0f, y - 32.0f },
                            { x - 32.0f, y + 32.0f }
                        });
                        cell.edge_id[WEST] = edge_id;
                        cell.edge_exists[WEST] = true;
                    }
                }

                // If this cell has no eastern neighbour it needs eastern edge
                if (validIndex(e, cells) && !cells[e].exists) {
                    if (validIndex(n, cells) && cells[n].edge_exists[EAST]) {
                        // Norther neighbour has a western edge, so grow it downwards
                        m_edges[cells[n].edge_id[EAST]].end.y += 64.0f;
                        cell.edge_exists[EAST] = true;
                        cell.edge_id[EAST] = cells[n].edge_id[EAST];
                    } else {
                        // Create new edge
                        auto edge_id = m_edges.size();
                        m_edges.push_back({
                            { x + 32.0f, y - 32.0f },
                            { x + 32.0f, y + 32.0f }
                        });
                        cell.edge_id[EAST] = edge_id;
                        cell.edge_exists[EAST] = true;
                    }
                }

                // If this cell has no northern neighbour it needs northern edge
                if (validIndex(n, cells) && !cells[n].exists) {
                    if (validIndex(w, cells) && cells[w].edge_exists[NORTH]) {
                        // Western neighbour has a northern edge, so grow it eastwards
                        m_edges[cells[w].edge_id[NORTH]].end.x += 64.0f;
                        cell.edge_exists[NORTH] = true;
                        cell.edge_id[NORTH] = cells[w].edge_id[NORTH];
                    } else {
                        // Create new edge
                        auto edge_id = m_edges.size();
                        m_edges.push_back({
                            { x - 32.0f, y - 32.0f },
                            { x + 32.0f, y - 32.0f }
                        });
                        cell.edge_id[NORTH] = edge_id;
                        cell.edge_exists[NORTH] = true;
                    }
                }

                // If this cell has no southern neighbour it needs southern edge
                if (validIndex(s, cells) && !cells[s].exists) {
                    if (validIndex(w, cells) && cells[w].edge_exists[SOUTH]) {
                        // Western neighbour has a southern edge, so grow it eastwards
                        m_edges[cells[w].edge_id[SOUTH]].end.x += 64.0f;
                        cell.edge_exists[SOUTH] = true;
                        cell.edge_id[SOUTH] = cells[w].edge_id[SOUTH];
                    } else {
                        // Create new edge
                        auto edge_id = m_edges.size();
                        m_edges.push_back({
                            { x - 32.0f, y + 32.0f },
                            { x + 32.0f, y + 32.0f }
                        });
                        cell.edge_id[SOUTH] = edge_id;
                        cell.edge_exists[SOUTH] = true;
                    }
                }
            }
            i++;
        }
    }
    // for (const auto& e : m_edges) {
    //     std::cout << e.start << "->" << e.end << '\n';
    // }
}

const Boundary EdgePool::getWorldBoundary(EntityManager& entity_manager, GameEngine* engine) {
    Vec2 min(0.0f, 0.0f);
    Vec2 max(engine->window().getSize().x, engine->window().getSize().y);
    const Vec2 m_grid_cell_size{ 64, 64 };

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
    }

    return Boundary{ min, max };
}

bool EdgePool::validIndex(size_t index, std::vector<Cell> cells) {
    return cells.size() > 0 && index >= 0 && index < cells.size();
}
