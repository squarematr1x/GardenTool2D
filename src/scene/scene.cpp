#include "scene.hpp"

#include "../engine.hpp"
#include "../core/rectangle.hpp"
#include "../collision/physics.hpp"
#include "../collision/light.hpp"

Scene::Scene(GameEngine* engine)
    : m_engine(engine),
    m_grid_text(
        engine->assets().getFont("Arial"),
        "",
        12,
        Vec2(0, 0)
    ),
    m_pause_text(
        m_engine->assets().getFont("Arial"),
        "Paused",
        16,
        Vec2(0, 0)
    ) {
    m_grid_size = Vec2(width(), height());
}

Scene::Scene(GameEngine* engine, const std::string& level_path)
    : m_engine(engine),
    m_level_path(level_path),
    m_grid_text(
        engine->assets().getFont("Arial"),
        "",
        12,
        Vec2(0, 0)
    ),
    m_pause_text(
        m_engine->assets().getFont("Arial"),
        "Paused",
        16,
        Vec2(0, 0)
    ) {
    m_grid_size = Vec2(width(), height());
}

unsigned int Scene::width() const {
    return m_engine->window().width();
}

unsigned int Scene::height() const {
    return m_engine->window().heigh();
}

Vec2 Scene::getCenter() const {
    return m_engine->window().getView().getCenter();
}

Vec2 Scene::fitToGrid(const Vec2& pos, bool mid_pixel) const {
    if (mid_pixel) {
        return Vec2(
            floorf(pos.x/m_grid_cell_size.x)*m_grid_cell_size.x + m_grid_cell_size.x/2,
            floorf(pos.y/m_grid_cell_size.y)*m_grid_cell_size.y + m_grid_cell_size.y/2
        );
    }
    return Vec2(
        floorf(pos.x/m_grid_cell_size.x)*m_grid_cell_size.x,
        floorf(pos.y/m_grid_cell_size.y)*m_grid_cell_size.y
    );
}

void Scene::drawLine(const Vec2& p1, const Vec2& p2) {
    VertexArray line(LINES, 2UL);
    line.setVertexAt(0, {p1.x, p1.y}, Color(255, 255, 255));
    line.setVertexAt(1, {p2.x, p2.y}, Color(255, 255, 255));

    m_engine->window().draw(line);
}

void Scene::renderGrid(bool show_coordinates) {
    const Vec2 center = getCenter();

    const auto w = m_grid_size.x;
    const auto h = m_grid_size.y;

    const auto left_x = center.x - w/2;
    const auto right_x = left_x + w + m_grid_cell_size.x;
    const auto next_grid_x = left_x - (static_cast<int>(left_x) % static_cast<int>(m_grid_cell_size.x));

    const auto up_y = center.y - h/2;
    const auto low_y = up_y + h + m_grid_cell_size.y;
    const auto next_grid_y = up_y - (static_cast<int>(up_y) % static_cast<int>(m_grid_cell_size.y));

    VertexArray vertices(LINES);

    for (auto x = next_grid_x; x < right_x; x += m_grid_cell_size.x) {
        addLine(Vec2(x, up_y), Vec2(x, low_y), vertices);
    }

    for (auto y = next_grid_y; y < low_y; y += m_grid_cell_size.y) {
        addLine(Vec2(left_x, y), Vec2(right_x, y), vertices);

        if (!show_coordinates) {
            continue;
        }

        for (float x = next_grid_x; x < right_x; x += m_grid_cell_size.x) {
            const auto x_cell = std::to_string(static_cast<int>(x)/static_cast<int>(m_grid_cell_size.x));
            const auto y_cell = std::to_string(static_cast<int>(y)/static_cast<int>(m_grid_cell_size.y));
            const auto x_offset = 3;
            const auto y_offset = 2;
            m_grid_text.setString("(" + x_cell + "," + y_cell + ")");
            m_grid_text.setPosition(x + x_offset, h - y - m_grid_cell_size.y + y_offset);
            m_engine->window().draw(m_grid_text);
        }
    }
    m_engine->window().draw(vertices);

    renderSelectedGridCell();
}

void Scene::renderActiveGridCell() {
    auto world_pos = worldPos();
    auto grid_pos = fitToGrid(world_pos, false);

    Rectangle active_cell;
    active_cell.setFillColor(Color(255, 255, 255, 128));
    active_cell.setPosition(grid_pos.x, grid_pos.y);
    active_cell.setSize(m_grid_cell_size.x, m_grid_cell_size.y);

    m_engine->window().draw(active_cell);
}

void Scene::renderSelectedGridCell() {
    for (const auto& pos : m_engine->allSelectedPos()) {
        const auto adjusted_pos = fitToGrid(pos, false);
        Rectangle active_cell;
        active_cell.setFillColor(Color(196, 196, 255, 156));
        active_cell.setPosition(adjusted_pos.x, adjusted_pos.y);
        active_cell.setSize(m_grid_cell_size.x, m_grid_cell_size.y);

        m_engine->window().draw(active_cell);
    }
}

void Scene::renderBBoxes() {
    VertexArray vertices(LINES);
    for (const auto& e : m_entity_manager.getEntities()) {
        if (!e->hasComponent<CBBox>()) {
            continue;
        }
        const auto box = e->getComponent<CBBox>();
        const auto pos = e->getComponent<CTransform>().pos;
        const auto box_color = e->hasComponent<CTrigger>() ? Color(0, 255, 0) : Color(255, 255, 255);

        vertices.append({pos.x - box.half_size.x, pos.y - box.half_size.y}, box_color);
        vertices.append({pos.x + box.half_size.x, pos.y - box.half_size.y}, box_color);
        vertices.append({pos.x + box.half_size.x, pos.y - box.half_size.y}, box_color);
        vertices.append({pos.x + box.half_size.x, pos.y + box.half_size.y}, box_color);
        vertices.append({pos.x + box.half_size.x, pos.y + box.half_size.y}, box_color);
        vertices.append({pos.x - box.half_size.x, pos.y + box.half_size.y}, box_color);
        vertices.append({pos.x - box.half_size.x, pos.y + box.half_size.y}, box_color);
        vertices.append({pos.x - box.half_size.x, pos.y - box.half_size.y}, box_color);
    }

    for (const auto& e : m_pool.getEdges()) {
        vertices.append({e.start}, Color(255, 75, 10));
        vertices.append({e.end}, Color(255, 75, 10));
    }
    m_engine->window().draw(vertices);
}

void Scene::renderCursor() {
    // TODO: Implement cursor rendering here
}

void Scene::addHpBar(std::shared_ptr<Entity> e) {
    if (!e->hasComponent<CHealth>() || !e->hasComponent<CAnimation>()) {
        return;
    }

    const auto size = e->getComponent<CAnimation>().animation.getSize();
    const auto pos = e->getComponent<CTransform>().pos;
    constexpr auto y_offset{ 16.0f };
    constexpr auto hbar_h{ 8.0f };
    const auto x_size = size.x*e->getComponent<CHealth>().percentage;

    m_hp_bars.append({pos.x - size.x/2, pos.y - size.y/2 - y_offset + hbar_h}, Color(0, 0, 0));
    m_hp_bars.append({pos.x + size.x/2, pos.y - size.y/2 - y_offset + hbar_h}, Color(0, 0, 0));
    m_hp_bars.append({pos.x + size.x/2, pos.y - size.y/2 - y_offset}, Color(0, 0, 0));
    m_hp_bars.append({pos.x + size.x/2, pos.y - size.y/2 - y_offset}, Color(0, 0, 0));
    m_hp_bars.append({pos.x - size.x/2, pos.y - size.y/2 - y_offset}, Color(0, 0, 0));
    m_hp_bars.append({pos.x - size.x/2, pos.y - size.y/2 - y_offset + hbar_h}, Color(0, 0, 0));

    m_hp_bars.append({pos.x - size.x/2, pos.y - size.y/2 - y_offset + hbar_h}, Color(255, 0, 0));
    m_hp_bars.append({pos.x - size.x/2 + x_size, pos.y - size.y/2 - y_offset + hbar_h}, Color(255, 0, 0));
    m_hp_bars.append({pos.x - size.x/2 + x_size, pos.y - size.y/2 - y_offset}, Color(255, 0, 0));
    m_hp_bars.append({pos.x - size.x/2 + x_size, pos.y - size.y/2 - y_offset}, Color(255, 0, 0));
    m_hp_bars.append({pos.x - size.x/2, pos.y - size.y/2 - y_offset}, Color(255, 0, 0));
    m_hp_bars.append({pos.x - size.x/2, pos.y - size.y/2 - y_offset + hbar_h}, Color(255, 0, 0));
}

void Scene::addHighlight(std::shared_ptr<Entity> e) {
    if (!e->hasComponent<CBBox>()) {
        return;
    }

    const auto box = e->getComponent<CBBox>();
    const auto pos = e->getComponent<CTransform>().pos;
    const auto box_color{ Color(255, 255, 255) };

    m_highlights.append({pos.x - box.half_size.x, pos.y - box.half_size.y}, box_color);
    m_highlights.append({pos.x + box.half_size.x, pos.y - box.half_size.y}, box_color);
    m_highlights.append({pos.x + box.half_size.x, pos.y - box.half_size.y}, box_color);
    m_highlights.append({pos.x + box.half_size.x, pos.y + box.half_size.y}, box_color);
    m_highlights.append({pos.x + box.half_size.x, pos.y + box.half_size.y}, box_color);
    m_highlights.append({pos.x - box.half_size.x, pos.y + box.half_size.y}, box_color);
    m_highlights.append({pos.x - box.half_size.x, pos.y + box.half_size.y}, box_color);
    m_highlights.append({pos.x - box.half_size.x, pos.y - box.half_size.y}, box_color);
}

void Scene::renderHpBars() {
    m_engine->window().draw(m_hp_bars);
    m_hp_bars.clear();
}

void Scene::renderHighlights() {
    m_engine->window().draw(m_highlights);
    m_highlights.clear();
}

void Scene::renderInfoAI(std::shared_ptr<Entity> e, std::shared_ptr<Entity> player) {
    const auto e_pos = e->getComponent<CTransform>().pos;
    const auto p_pos = player->getComponent<CTransform>().pos;
    Circle center;
    center.setFillColor(Color(255, 255, 255));
    center.setRadius(4);
    center.setOrigin(2, 2);
    center.setPosition(e_pos.x, e_pos.y);
    m_engine->window().draw(center);
    center.setPosition(p_pos.x, p_pos.y);
    m_engine->window().draw(center);

    if (e->hasComponent<CFollowPlayer>()) {
        if (e->getComponent<CFollowPlayer>().detected == true) {
            drawLine(e_pos, p_pos);
        }
    }

    if (e->hasComponent<CPatrol>()) {
        const auto positions = e->getComponent<CPatrol>().positions;
        Circle patrol_pos;
        patrol_pos.setFillColor(Color(255, 255, 255));
        patrol_pos.setRadius(4);
        patrol_pos.setOrigin(2, 2);

        for (const auto& pos : positions) {
            patrol_pos.setPosition(pos.x, pos.y);
            m_engine->window().draw(patrol_pos);
        }
    }
}

void Scene::renderLights(const Vec2& source, const std::vector<std::tuple<float, float, float>>& visibility_points) {
    if (m_visibility_points.size() == 0) {
        return;
    }

    VertexArray vertices(TRIANGLE);
    Color light_color(255, 255, 255, 185);
    size_t n = m_visibility_points.size();

    for (size_t i = 0; i < n - 1; i++) {
        vertices.append(source, light_color );
        vertices.append({ std::get<1>(visibility_points[i]), std::get<2>(visibility_points[i]) }, light_color );
        vertices.append({ std::get<1>(visibility_points[i + 1]), std::get<2>(visibility_points[i + 1]) }, light_color );
    }

    vertices.append(source, light_color );
    vertices.append({ std::get<1>(visibility_points[n - 1]), std::get<2>(visibility_points[n - 1]) }, light_color );
    vertices.append({ std::get<1>(visibility_points[0]), std::get<2>(visibility_points[0]) }, light_color );

    m_engine->window().draw(vertices);
}

void Scene::renderCommon(std::shared_ptr<Entity> player) {
    if (m_draw_textures) {
        VertexArray vertices(TRIANGLE);
        for (auto e : m_entity_manager.getEntities()) {
            if (!e->hasComponent<CAnimation>()) {
                continue;
            }
            auto& transform = e->getComponent<CTransform>();
            auto& sprite = e->getComponent<CAnimation>().animation.getTextureRect();
            if (transform.transformable) {
                sprite.setRotation(transform.angle);
                sprite.setPosition(transform.pos.x, transform.pos.y);
                sprite.setScale(transform.scale.x, transform.scale.y);
                m_engine->window().draw(sprite);
            } else {
                addVertexData(transform.pos, sprite.getTextureRect(), vertices);
            }

            if (m_show_hp) {
                addHpBar(e);
            }

            if (m_show_ai_info && e->tag() == Tag::ENEMY) {
                renderInfoAI(e, player);
            }

            if (e->hasComponent<CInteractable>() && e->getComponent<CInteractable>().highlight) {
                addHighlight(e);
            }
        }
        // Draw vertex array
        m_engine->window().draw(vertices, m_engine->assets().getTexture("Tilemap"));

        renderLights(worldPos(), m_visibility_points);
        renderHighlights();
        renderHpBars();
    }

    if (m_draw_grid || m_engine->editMode()) {
        renderGrid();
        renderActiveGridCell();
    }

    if (m_draw_collision || m_engine->editMode()) {
        renderBBoxes();
    }

    if (m_paused) {
        renderPauseText();
    }
}

void Scene::updateZoom(float scroll_delta) {
    m_zoom.value = 2.0f;
    auto level = 1;
    if (scroll_delta > 0.0f) {
        m_zoom.value = 0.5f;
        level = -1;
    }
    const auto new_level = m_zoom.level + level;
    if (new_level >= -m_zoom.max_level && new_level <= m_zoom.max_level) {
        m_zoom.level = new_level;
        // a(n) = n^2 - 1
        m_zoom.magnitude = powf(2.0f, static_cast<float>(m_zoom.level)) - 1.0f;

        if (m_zoom.level > 0) {
            m_grid_size = Vec2(
                width()*powf(2, m_zoom.level),
                height()*powf(2, m_zoom.level)
            );
        } else {
            m_grid_size = Vec2(width(), height());
        }
    }
}

Vec2 Scene::worldPos() {
    const auto world_pos = m_engine->window().mapPixelToCoords(m_mouse_pos);
    return Vec2(world_pos.x, world_pos.y);
}

void Scene::renderPauseText() {
    const auto view = m_engine->window().getView();
    const auto default_view = m_engine->window().getDefaultView(); // Ignore zoom level etc.
    m_engine->window().setView(default_view);

    const auto w = static_cast<float>(width());
    constexpr auto h = 32.0f;
    const auto color = Color(0, 0, 0);
    VertexArray vertices{ TRIANGLE, 6 };

    vertices.setVertexAt(0, {0.0f, 0.0f}, color);
    vertices.setVertexAt(1, {w, 0.0f}, color);
    vertices.setVertexAt(2, {w, h}, color);
    vertices.setVertexAt(3, {w, h}, color);
    vertices.setVertexAt(4, {0.0f, h}, color);
    vertices.setVertexAt(5, {0.0f, 0.0f}, color);
    
    m_engine->window().draw(vertices);

    m_pause_text.setPosition(w/2 - m_pause_text.getLocalBounds().width/2, 5.0f);

    m_engine->window().draw(m_pause_text);
    m_engine->window().setView(view); // Restore previous view
}

bool Scene::targetReached(const Vec2& pos, const Vec2& target) const {
    auto distance = pos.distance(target);
    return fabs(distance) <= 5.0f;
}

void Scene::addVertexData(const Vec2& pos, const Rect<float>& texture_rect, VertexArray& vertices) {
    // Add all entities that are not individually target of some transform into same vertex array to reduce draw() calls
    const auto half_w = texture_rect.width/2.0f;
    const auto half_h = texture_rect.height/2.0f;
 
    vertices.append(
        Vec2(pos.x - half_w, pos.y - half_h),
        Vec2(texture_rect.left, texture_rect.top)
    );
    vertices.append(
        Vec2(pos.x + half_w, pos.y - half_h),
        Vec2(texture_rect.left + texture_rect.width, texture_rect.top)
    );
    vertices.append(
        Vec2(pos.x + half_w, pos.y + half_h),
        Vec2(texture_rect.left + texture_rect.width, texture_rect.top + texture_rect.height)
    );
    vertices.append(
        Vec2(pos.x - half_w, pos.y - half_h),
        Vec2(texture_rect.left, texture_rect.top)
    );
    vertices.append(
        Vec2(pos.x + half_w, pos.y + half_h),
        Vec2(texture_rect.left + texture_rect.width, texture_rect.top + texture_rect.height)
    );
    vertices.append(
        Vec2(pos.x - half_w, pos.y  + half_h),
        Vec2(texture_rect.left, texture_rect.top + texture_rect.height)
    );
}

void Scene::addLine(const Vec2& p1, const Vec2& p2, VertexArray& vertices) {
    vertices.append(Vec2(p1.x, p1.y), Color(255, 255, 255));
    vertices.append(Vec2(p2.x, p2.y), Color(255, 255, 255));
}

Vec2 Scene::gridPos(const Vec2& pos, const Vec2& size) const {
    auto fixed_size = Vec2(fmaxf(size.x, 64.0f), fmaxf(size.y, 64.0f));

    return Vec2(
        round((pos.x - (fixed_size.x/2))/m_grid_cell_size.x),
        round((static_cast<float>(height()) - (pos.y + (fixed_size.y/2)))/m_grid_cell_size.y)
    );
}

Vec2 Scene::gridToMidPixel(float grid_x, float grid_y, std::shared_ptr<Entity> entity) const {
    const auto mod_h = static_cast<float>(static_cast<int>(height())%static_cast<int>(m_grid_cell_size.y));
    const auto grid_h = mod_h != 0 ? height() + m_grid_cell_size.y - mod_h : height();

    if (entity->hasComponent<CAnimation>()) {
        const auto animation_size = entity->getComponent<CAnimation>().animation.getSize();
        const auto x = grid_x*m_grid_cell_size.x + (animation_size.x/2.0f);
        const auto y = grid_h - (grid_y*m_grid_cell_size.y + (animation_size.y/2.0f));

        return Vec2(x, y);
    }
    if (entity->hasComponent<CBBox>()) {
        const auto half_size = entity->getComponent<CBBox>().half_size;
        const auto x = grid_x*m_grid_cell_size.x + half_size.x;
        const auto y = grid_h - (grid_y*m_grid_cell_size.y + half_size.y);

        return Vec2(x, y);
    }
    return Vec2(0, 0);
}

const std::vector<std::string> Scene::getLayerNames() const {
    std::vector<std::string> layer_names;
    for (const auto& layer : m_background_layers) {
        layer_names.push_back(layer.getName());
    }
    return layer_names;
}

void Scene::sDoActionCommon(const Action& action) {
    if (action.getType() == ActionType::START) {
        switch (action.getName()) {
            case ActionName::TOGGLE_TEXTURE: m_draw_textures = !m_draw_textures; break;
            case ActionName::TOGGLE_COLLISION: m_draw_collision = !m_draw_collision; break;
            case ActionName::TOGGLE_GRID: m_draw_grid = !m_draw_grid; break;
            case ActionName::TOGGLE_HEALTH: m_show_hp = !m_show_hp; break;
            case ActionName::TOGGLE_LIGHT: {
                m_draw_light = !m_draw_light;
                if (m_draw_light) {
                    std::vector<Edge> edges = m_pool.getEdges();
                    // Add northern border
                    edges.push_back({
                        Vec2(worldPos().x - width(), worldPos().y - height()),
                        Vec2(worldPos().x + width(), worldPos().y - height())
                    });
                    // Add souther border
                    edges.push_back({
                        Vec2(worldPos().x - width(), worldPos().y + height()),
                        Vec2(worldPos().x + width(), worldPos().y + height())
                    });
                    // Add western border
                    edges.push_back({
                        Vec2(worldPos().x - width(), worldPos().y - height()),
                        Vec2(worldPos().x - width(), worldPos().y + height())
                    });
                    // Add eastern border
                    edges.push_back({
                        Vec2(worldPos().x + width(), worldPos().y - height()),
                        Vec2(worldPos().x + width(), worldPos().y + height())
                    });
                    m_visibility_points = light::constructVisibilityPoints(worldPos(), 1000.0f, edges);
                } else {
                    m_visibility_points.clear();
                }
                break;
            }
            case ActionName::PAUSE: setPaused(!m_paused); break;
            case ActionName::QUIT: {
                if (m_free_camera) {
                    m_free_camera = false;
                    break;
                }
                onEnd();
                break;
            }
            case ActionName::MOUSE_MOVE: m_mouse_pos = action.pos; m_mouse_shape.setPosition(m_mouse_pos.x, m_mouse_pos.y); break;
            case ActionName::MOUSE_SCROLL: updateZoom(action.delta); break;
            case ActionName::MIDDLE_CLICK: {
                m_free_camera = true;
                m_drag_pos = m_mouse_pos;
                m_middle_mouse_pressed = true;
                break;
            }
            case ActionName::L_SYSTEM: { m_system_key_pressed = true; break; }
            case ActionName::LEFT_CLICK: {
                if (!m_engine->editMode()) {
                    break;
                }

                const auto world_pos = worldPos();
                m_engine->pushSelectedPos(fitToGrid(world_pos), !m_system_key_pressed);
                for (auto e : m_entity_manager.getEntities()) {
                    if (physics::isInside(world_pos, e)) {
                        m_engine->pushSelectedEntityId(e->id(), !m_system_key_pressed);
                        if (e->hasComponent<CDraggable>()) {
                            e->getComponent<CDraggable>().dragged = true;
                        }
                        break;
                    }
                }
                break;
            }
            case ActionName::TOGGLE_LEVEL_EDITOR: {
                m_engine->toggleEditMode();
                m_paused = m_engine->editMode();
                break;
            }
            default: break;
        } 
    }
    if (action.getType() == ActionType::END) {
        switch (action.getName()) {
            case ActionName::L_SYSTEM: m_system_key_pressed = false; break;
            case ActionName::MIDDLE_CLICK: m_middle_mouse_pressed = false; break;
            case ActionName::LEFT_CLICK: {
                if (m_engine->editMode()) {
                    auto world_pos = worldPos();
                    for (auto e : m_entity_manager.getEntities()) {
                        if (e->hasComponent<CDraggable>() && physics::isInside(world_pos, e)) {
                            e->getComponent<CDraggable>().dragged = false;
                            e->getComponent<CTransform>().pos = (fitToGrid(world_pos));
                        }
                    }
                    break;
                }
            }
            default: break;
        }
    }
}

void Scene::sPan(View& view) {
    constexpr auto pan_speed_coeff = 4.0f;
    auto diff = Vec2(m_drag_pos - m_mouse_pos)/pan_speed_coeff;
    view.setCenter(view.getCenter().x + diff.x, view.getCenter().y + diff.y);
    m_engine->window().setView(view);
}

void Scene::sZoom(View& view) {
    if (m_zoom.level != m_zoom.prev_level) {
        m_zoom.prev_level = m_zoom.level;
        view.zoom(m_zoom.value);
    }
}
