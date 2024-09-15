#include "scene.hpp"
#include "../engine.hpp"

Scene::Scene(GameEngine* engine)
    : m_engine(engine) {
    m_grid = Vec2(width(), height());
}

Scene::Scene(GameEngine* engine, const std::string& level_path)
    : m_engine(engine), m_level_path(level_path) {
    m_grid = Vec2(width(), height());
}

size_t Scene::width() const {
    return m_engine->window().getSize().x;
}

size_t Scene::height() const {
    return m_engine->window().getSize().y;
}

Vec2 Scene::fitToGrid(const Vec2& pos) const {
    return Vec2(
        floorf(pos.x/m_grid_size.x)*m_grid_size.x + m_grid_size.x/2,
        floorf(pos.y/m_grid_size.y)*m_grid_size.y + m_grid_size.y/2
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
    const float w = m_grid.x;
    const float h = m_grid.y;

    const float left_x = m_engine->window().getView().getCenter().x - w / 2;
    const float right_x = left_x + w + m_grid_size.x;
    const float next_grid_x = left_x - (static_cast<int>(left_x) % static_cast<int>(m_grid_size.x));

    const float up_y = m_engine->window().getView().getCenter().y - h / 2;
    const float low_y = up_y + h + m_grid_size.y;
    const float next_grid_y = up_y - (static_cast<int>(up_y) % static_cast<int>(m_grid_size.y));

    sf::VertexArray vertices(sf::Lines);

    for (float x = next_grid_x; x < right_x; x += m_grid_size.x) {
        addLine(Vec2(x, up_y), Vec2(x, low_y), vertices);
    }

    for (float y = next_grid_y; y < low_y; y += m_grid_size.y) {
        addLine(Vec2(left_x, y), Vec2(right_x, y), vertices);

        if (!show_coordinates) {
            continue;
        }

        for (float x = next_grid_x; x < right_x; x += m_grid_size.x) {
            const std::string x_cell = std::to_string(static_cast<int>(x) / static_cast<int>(m_grid_size.x));
            const std::string y_cell = std::to_string(static_cast<int>(y) / static_cast<int>(m_grid_size.y));
            const int x_offset = 3;
            const int y_offset = 2;
            m_grid_text.setString("(" + x_cell + "," + y_cell + ")");
            m_grid_text.setPosition(x + x_offset, h - y - m_grid_size.y + y_offset);
            m_engine->window().draw(m_grid_text);
        }
    }
    m_engine->window().draw(vertices);
}

void Scene::renderBBoxes() {
    sf::VertexArray vertices(sf::Lines);
    for (auto e : m_entity_manager.getEntities()) {
        if (!e->hasComponent<CBBox>()) {
            continue;
        }
        auto box = e->getComponent<CBBox>();
        auto pos = e->getComponent<CTransform>().pos;
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

    auto size = e->getComponent<CAnimation>().animation.getSize();
    auto pos = e->getComponent<CTransform>().pos;
    constexpr float y_offset{ 16.0f };
    constexpr float hbar_h{ 8.0f };
    const float x_size = size.x*e->getComponent<CHealth>().percentage;

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

void Scene::renderHpBars() {
    m_engine->window().draw(m_hp_bars);
    m_hp_bars.clear();
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
        auto positions = e->getComponent<CPatrol>().positions;
        sf::CircleShape patrol_pos;
        patrol_pos.setFillColor(sf::Color(255, 255, 255));
        patrol_pos.setRadius(4);
        patrol_pos.setOrigin(2, 2);

        for (const Vec2& pos : positions) {
            patrol_pos.setPosition(pos.x, pos.y);
            m_engine->window().draw(patrol_pos);
        }
    }
}

void Scene::updateZoom(float scroll_delta) {
    m_zoom.value = 2.0f;
    int level = 1;
    if (scroll_delta > 0.0f) {
        m_zoom.value = 0.5f;
        level = -1;
    }
    const int new_level = m_zoom.level + level;
    if (new_level >= -m_zoom.max_level && new_level <= m_zoom.max_level) {
        m_zoom.level = new_level;
        // a(n) = n^2 - 1
        m_zoom.magnitude = powf(2.0f, static_cast<float>(m_zoom.level)) - 1.0f;

        if (m_zoom.level > 0) {
            m_grid = Vec2(
                width() * powf(2, m_zoom.level),
                height() * powf(2, m_zoom.level)
            );
        } else {
            m_grid = Vec2(width(), height());
        }
    }
}

Vec2 Scene::worldPos(const Vec2& room) {
    Vec2 world_pos = {
        m_mouse_pos.x + ((m_mouse_pos.x - width()/2) * m_zoom.magnitude),
        m_mouse_pos.y + ((m_mouse_pos.y - height()/2) * m_zoom.magnitude)
    };
    Vec2 room_pos = {
        world_pos.x + room.x * width(),
        world_pos.y + room.y * height()
    };

    return room_pos;
}

void Scene::renderPauseText() {
    sf::View view = m_engine->window().getView();
    sf::View default_view = m_engine->window().getDefaultView(); // Ignore zoom level etc.
    m_engine->window().setView(default_view);

    const float w = static_cast<float>(width());
    constexpr float h = 32.0f;
    const sf::Color color = sf::Color(0, 0, 0);
    sf::VertexArray vertices{ sf::Triangles, 6 };

    vertices[0] = {{0.0f, 0.0f}, color};
    vertices[1] = {{w, 0.0f}, color};
    vertices[2] = {{w, h}, color};
    vertices[3] = {{w, h}, color};
    vertices[4] = {{0.0f, h}, color};
    vertices[5] = {{0.0f, 0.0f}, color};
    
    m_engine->window().draw(vertices);

    sf::Text text;
    text.setCharacterSize(16);
    text.setFont(m_engine->assets().getFont("Arial"));
    text.setString("Paused");
    text.setPosition(w/2 - (text.getLocalBounds().width/2), 5.0f);

    m_engine->window().draw(text);
    m_engine->window().setView(view); // Restore previous view
}

bool Scene::targetReached(const Vec2& pos, const Vec2& target) const {
    float distance = pos.distance(target);
    return fabs(distance) <= 5.0f;
}

void Scene::addVertexData(const Vec2& pos, const sf::IntRect& texture_rect_in, sf::VertexArray& vertices) {
    // Add all entities that are not individually target of some transform into same vertex array to reduce draw() calls
    auto texture_rect = sf::FloatRect(texture_rect_in);
    float half_w = texture_rect.width / 2.0f;
    float half_h = texture_rect.height / 2.0f;
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
