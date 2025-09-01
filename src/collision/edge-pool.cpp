#include "edge-pool.hpp"

#include <unordered_set>

#include "../ecs/entity-manager.hpp"
#include "../engine.hpp"

void EdgePool::constructPool(EntityManager& entity_manager, GameEngine* engine) {
    std::unordered_set<Vec2, Vec2Hasher> vertices;

    for (const auto& e : entity_manager.getEntities(Tag::TILE)) {
        if (!e->hasComponent<CBBox>()) {
            continue;
        }
        vertices.insert(e->getComponent<CTransform>().pos);
    }

    auto boundaries = getWorldBoundary(entity_manager, engine);
    std::vector<Cell> cells;
    int w = 0;

    // Init
    for (float y = boundaries.min.y; y <= boundaries.max.y; y += 64.0f) {
        for (float x = boundaries.min.x; x <= boundaries.max.x; x+= 64.0f) {
            Cell cell;
            cells.push_back(cell);
        }
        w++;
    }
    if (cells.size() <= 0) {
        return;
    }

    int i = 0;
    for (float x = boundaries.min.x; x <= boundaries.max.x; x += 64.0f) {
        for (float y = boundaries.min.y; y <= boundaries.max.y; y+= 64.0f) {
            auto cell = cells[i];
            size_t w = i - 1;
            size_t e = i + 1;
            size_t n = i - w;
            size_t s = i + w;
            if (auto search = vertices.find(Vec2(x, y)); search != vertices.end()) {
                cell.exists = true;
                
                // West
                if (auto search = vertices.find(Vec2(x - 64.0f, y)); search != vertices.end() && validIndex(w, cells)) {
                    cell.edge_exists[0] = true;
                    cell.edge_id[0] = cells[w].edge_id[0];
                } else {
                    int edge_id = m_edges.size();
                    m_edges.push_back({
                        { x - 32.0f, y - 32.0f },
                        { x - 32.0f, y + 32.0f }
                    });
                    cell.edge_id[0] = edge_id;
                }
                // East
                if (auto search = vertices.find(Vec2(x - 64.0f, y)); search != vertices.end() && validIndex(e, cells)) {
                    cell.edge_exists[1] = true;
                    cell.edge_id[1] = cells[e].edge_id[1];
                } else {
                    int edge_id = m_edges.size();
                    m_edges.push_back({
                        { x + 32.0f, y - 32.0f },
                        { x + 32.0f, y + 32.0f }
                    });
                    cell.edge_id[1] = edge_id;
                }
                // North
                if (auto search = vertices.find(Vec2(x, y - 64.0f)); search != vertices.end() && validIndex(n, cells)) {
                    cell.edge_exists[2] = true;
                    cell.edge_id[2] = cells[n].edge_id[2];
                } else {
                    int edge_id = m_edges.size();
                    m_edges.push_back({
                        { x - 32.0f, y - 32.0f },
                        { x + 32.0f, y - 32.0f }
                    });
                    cell.edge_id[2] = edge_id;
                }
                // South
                if (auto search = vertices.find(Vec2(x, y + 64.0f)); search != vertices.end() && validIndex(s, cells)) {
                    cell.edge_exists[3] = true;
                    cell.edge_id[3] = cells[s].edge_id[3];
                } else {
                    int edge_id = m_edges.size();
                    m_edges.push_back({
                        { x - 32.0f, y + 32.0f },
                        { x + 32.0f, y + 32.0f }
                    });
                    cell.edge_id[3] = edge_id;
                }
            }
            i++;
        }
    }
}

const Boundary EdgePool::getWorldBoundary(EntityManager& entity_manager, GameEngine* engine) {
    Vec2 min(0.0f, 0.0f);
    Vec2 max(engine->window().getSize().x, engine->window().getSize().y);
    const Vec2 m_grid_cell_size{ 64, 64 };

    for (const auto& e : entity_manager.getEntities(Tag::TILE)) {
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
