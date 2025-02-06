#include "scene.hpp"
#include "../engine.hpp"

Scene::Scene(GameEngine* engine)
    : m_engine(engine) {
    m_grid_size = Vec2(width(), height());
}

Scene::Scene(GameEngine* engine, const std::string& level_path)
    : m_engine(engine), m_level_path(level_path) {
    m_grid_size = Vec2(width(), height());
}

size_t Scene::width() const {
    return m_engine->window().getSize().x;
}

size_t Scene::height() const {
    return m_engine->window().getSize().y;
}

Vec2 Scene::getCenter() const {
    const auto center = m_engine->window().getView().getCenter();
    return Vec2(center.x, center.y);
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
    const sf::Vertex line[] = {
        {{p1.x, p1.y}, sf::Color(255, 255, 255)},
        {{p2.x, p2.y}, sf::Color(255, 255, 255)}
    };
    m_engine->window().draw(line, 2, sf::Lines);
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

    sf::VertexArray vertices(sf::Lines);

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
            m_engine->window().draw(m_grid_text.getText());
        }
    }
    m_engine->window().draw(vertices);

    renderSelectedGridCell();
}

void Scene::renderActiveGridCell() {
    auto world_pos = worldPos();
    auto grid_pos = fitToGrid(world_pos, false);

    sf::RectangleShape active_cell;
    active_cell.setFillColor({255, 255, 255, 128});
    active_cell.setPosition({grid_pos.x, grid_pos.y});
    active_cell.setSize({m_grid_cell_size.x, m_grid_cell_size.y});

    m_engine->window().draw(active_cell);
}

void Scene::renderSelectedGridCell() {
    auto grid_pos = fitToGrid(m_selected_cell, false);
    sf::RectangleShape active_cell;
    active_cell.setFillColor({196, 196, 255, 156});
    active_cell.setPosition({grid_pos.x, grid_pos.y});
    active_cell.setSize({m_grid_cell_size.x, m_grid_cell_size.y});

    m_engine->window().draw(active_cell);
}

void Scene::renderBBoxes() {
    sf::VertexArray vertices(sf::Lines);
    for (const auto& e : m_entity_manager.getEntities()) {
        if (!e->hasComponent<CBBox>()) {
            continue;
        }
        const auto box = e->getComponent<CBBox>();
        const auto pos = e->getComponent<CTransform>().pos;
        const auto box_color{ sf::Color(255, 255, 255) };

        vertices.append({{pos.x - box.half_size.x, pos.y - box.half_size.y}, box_color});
        vertices.append({{pos.x + box.half_size.x, pos.y - box.half_size.y}, box_color});
        vertices.append({{pos.x + box.half_size.x, pos.y - box.half_size.y}, box_color});
        vertices.append({{pos.x + box.half_size.x, pos.y + box.half_size.y}, box_color});
        vertices.append({{pos.x + box.half_size.x, pos.y + box.half_size.y}, box_color});
        vertices.append({{pos.x - box.half_size.x, pos.y + box.half_size.y}, box_color});
        vertices.append({{pos.x - box.half_size.x, pos.y + box.half_size.y}, box_color});
        vertices.append({{pos.x - box.half_size.x, pos.y - box.half_size.y}, box_color});
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

    m_hp_bars.append({{pos.x - size.x/2, pos.y - size.y/2 - y_offset + hbar_h}, sf::Color(0, 0, 0)});
    m_hp_bars.append({{pos.x + size.x/2, pos.y - size.y/2 - y_offset + hbar_h}, sf::Color(0, 0, 0)});
    m_hp_bars.append({{pos.x + size.x/2, pos.y - size.y/2 - y_offset}, sf::Color(0, 0, 0)});
    m_hp_bars.append({{pos.x + size.x/2, pos.y - size.y/2 - y_offset}, sf::Color(0, 0, 0)});
    m_hp_bars.append({{pos.x - size.x/2, pos.y - size.y/2 - y_offset}, sf::Color(0, 0, 0)});
    m_hp_bars.append({{pos.x - size.x/2, pos.y - size.y/2 - y_offset + hbar_h}, sf::Color(0, 0, 0)});

    m_hp_bars.append({{pos.x - size.x/2, pos.y - size.y/2 - y_offset + hbar_h}, sf::Color(255, 0, 0)});
    m_hp_bars.append({{pos.x - size.x/2 + x_size, pos.y - size.y/2 - y_offset + hbar_h}, sf::Color(255, 0, 0)});
    m_hp_bars.append({{pos.x - size.x/2 + x_size, pos.y - size.y/2 - y_offset}, sf::Color(255, 0, 0)});
    m_hp_bars.append({{pos.x - size.x/2 + x_size, pos.y - size.y/2 - y_offset}, sf::Color(255, 0, 0)});
    m_hp_bars.append({{pos.x - size.x/2, pos.y - size.y/2 - y_offset}, sf::Color(255, 0, 0)});
    m_hp_bars.append({{pos.x - size.x/2, pos.y - size.y/2 - y_offset + hbar_h}, sf::Color(255, 0, 0)});
}

void Scene::addHighlight(std::shared_ptr<Entity> e) {
    if (!e->hasComponent<CBBox>()) {
        return;
    }

    const auto box = e->getComponent<CBBox>();
    const auto pos = e->getComponent<CTransform>().pos;
    const auto box_color{ sf::Color(255, 255, 255) };

    m_highlights.append({{pos.x - box.half_size.x, pos.y - box.half_size.y}, box_color});
    m_highlights.append({{pos.x + box.half_size.x, pos.y - box.half_size.y}, box_color});
    m_highlights.append({{pos.x + box.half_size.x, pos.y - box.half_size.y}, box_color});
    m_highlights.append({{pos.x + box.half_size.x, pos.y + box.half_size.y}, box_color});
    m_highlights.append({{pos.x + box.half_size.x, pos.y + box.half_size.y}, box_color});
    m_highlights.append({{pos.x - box.half_size.x, pos.y + box.half_size.y}, box_color});
    m_highlights.append({{pos.x - box.half_size.x, pos.y + box.half_size.y}, box_color});
    m_highlights.append({{pos.x - box.half_size.x, pos.y - box.half_size.y}, box_color});
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
    sf::CircleShape center;
    center.setFillColor(sf::Color(255, 255, 255));
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
        sf::CircleShape patrol_pos;
        patrol_pos.setFillColor(sf::Color(255, 255, 255));
        patrol_pos.setRadius(4);
        patrol_pos.setOrigin(2, 2);

        for (const auto& pos : positions) {
            patrol_pos.setPosition(pos.x, pos.y);
            m_engine->window().draw(patrol_pos);
        }
    }
}

void Scene::renderCommon(std::shared_ptr<Entity> player) {
    if (m_draw_textures) {
        sf::VertexArray vertices(sf::Triangles);
        for (auto e : m_entity_manager.getEntities()) {
            if (!e->hasComponent<CAnimation>()) {
                continue;
            }
            auto& transform = e->getComponent<CTransform>();
            auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
            if (transform.transformable) {
                sprite.setRotation(transform.angle);
                sprite.setPosition(transform.pos.x, transform.pos.y);
                sprite.setScale(transform.scale.x, transform.scale.y);
                m_engine->window().draw(sprite.getSprite());
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
        sf::RenderStates states(&m_engine->assets().getTexture("Tilemap").getTexture());
        m_engine->window().draw(vertices, states);

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
    const auto world_pos = m_engine->window().mapPixelToCoords({
        static_cast<int>(m_mouse_pos.x),
        static_cast<int>(m_mouse_pos.y)
    });

    return Vec2(world_pos.x, world_pos.y);
}

void Scene::renderPauseText() {
    const auto view = m_engine->window().getView();
    const auto default_view = m_engine->window().getDefaultView(); // Ignore zoom level etc.
    m_engine->window().setView(default_view);

    const auto w = static_cast<float>(width());
    constexpr auto h = 32.0f;
    const auto color = sf::Color(0, 0, 0);
    sf::VertexArray vertices{ sf::Triangles, 6 };

    vertices[0] = {{0.0f, 0.0f}, color};
    vertices[1] = {{w, 0.0f}, color};
    vertices[2] = {{w, h}, color};
    vertices[3] = {{w, h}, color};
    vertices[4] = {{0.0f, h}, color};
    vertices[5] = {{0.0f, 0.0f}, color};
    
    m_engine->window().draw(vertices);

    Text text;
    text.setFont(m_engine->assets().getFont("Arial"));
    text.setString("Paused");
    text.setCharacterSize(16);
    text.setPosition(w/2 - (text.getLocalBounds().width/2), 5.0f);

    m_engine->window().draw(text.getText());
    m_engine->window().setView(view); // Restore previous view
}

bool Scene::targetReached(const Vec2& pos, const Vec2& target) const {
    auto distance = pos.distance(target);
    return fabs(distance) <= 5.0f;
}

void Scene::addVertexData(const Vec2& pos, const sf::IntRect& texture_rect_in, sf::VertexArray& vertices) {
    // Add all entities that are not individually target of some transform into same vertex array to reduce draw() calls
    const auto texture_rect = sf::FloatRect(texture_rect_in);
    const auto half_w = texture_rect.width/2.0f;
    const auto half_h = texture_rect.height/2.0f;
    vertices.append(sf::Vertex(
        sf::Vector2f(pos.x - half_w, pos.y - half_h),
        sf::Vector2f(texture_rect.left, texture_rect.top)
    ));
    vertices.append(sf::Vertex(
        sf::Vector2f(pos.x + half_w, pos.y - half_h),
        sf::Vector2f(texture_rect.left + texture_rect.width, texture_rect.top)
    ));
    vertices.append(sf::Vertex(
        sf::Vector2f(pos.x + half_w, pos.y + half_h),
        sf::Vector2f(texture_rect.left + texture_rect.width, texture_rect.top + texture_rect.height)
    ));
    vertices.append(sf::Vertex(
        sf::Vector2f(pos.x - half_w, pos.y - half_h),
        sf::Vector2f(texture_rect.left, texture_rect.top)
    ));
    vertices.append(sf::Vertex(
        sf::Vector2f(pos.x + half_w, pos.y + half_h),
        sf::Vector2f(texture_rect.left + texture_rect.width, texture_rect.top + texture_rect.height)
    ));
    vertices.append(sf::Vertex(
        sf::Vector2f(pos.x - half_w, pos.y  + half_h),
        sf::Vector2f(texture_rect.left, texture_rect.top + texture_rect.height)
    ));
}

void Scene::addLine(const Vec2& p1, const Vec2& p2, sf::VertexArray& vertices) {
    vertices.append({{p1.x, p1.y}, sf::Color(255, 255, 255)});
    vertices.append({{p2.x, p2.y}, sf::Color(255, 255, 255)});
}

Vec2 Scene::gridPos(const Vec2& pos) const {
    return Vec2(
        floor(pos.x/m_grid_cell_size.x),
        floor((static_cast<float>(height()) - pos.y)/m_grid_cell_size.y)
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
