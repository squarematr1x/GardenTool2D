#include "scene.hpp"
#include "engine.hpp"

size_t Scene::width() const {
    return m_engine->window().getSize().x;
}

size_t Scene::height() const {
    return m_engine->window().getSize().y;
}

void Scene::drawLine(const Vec2& p1, const Vec2& p2) {
    const sf::Vertex line[] = {
        {{p1.x, p1.y}, sf::Color(255, 255, 255)},
        {{p2.x, p2.y}, sf::Color(255, 255, 255)}
    };
    m_engine->window().draw(line, 2, sf::Lines);
}

void Scene::renderGrid(const Vec2& grid_size, sf::Text& grid_text) {
    const float left_x = m_engine->window().getView().getCenter().x - width() / 2;
    const float right_x = left_x + width() + grid_size.x;
    const float next_grid_x = left_x - (static_cast<int>(left_x) % static_cast<int>(grid_size.x));

    for (float x = next_grid_x; x < right_x; x += grid_size.x) {
        drawLine(Vec2(x, 0.0f), Vec2(x, height()));
    }

    for (float y = 0; y < height(); y += grid_size.y) {
        drawLine(Vec2(left_x, height() - y), Vec2(right_x, height() - y));

        for (float x = next_grid_x; x < right_x; x += grid_size.x) {
            std::string x_cell = std::to_string(static_cast<int>(x) / static_cast<int>(grid_size.x));
            std::string y_cell = std::to_string(static_cast<int>(y) / static_cast<int>(grid_size.y));
            grid_text.setString("(" + x_cell + "," + y_cell + ")");
            grid_text.setPosition(x + 3, height() - y - grid_size.y + 2);
            m_engine->window().draw(grid_text);
        }
    }
}

void Scene::renderBBoxes() {
    for (auto e : m_entity_manager.getEntities()) {
        if (e->hasComponent<CBBox>()) {
            auto& box = e->getComponent<CBBox>();
            auto& transform = e->getComponent<CTransform>();
            sf::RectangleShape rect;
            rect.setSize(sf::Vector2f(box.size.x-1, box.size.y-1));
            rect.setOrigin(sf::Vector2f(box.half_size.x, box.half_size.y));
            rect.setPosition(transform.pos.x, transform.pos.y);
            rect.setFillColor(sf::Color(255, 255, 255, 0));
            rect.setOutlineColor(sf::Color(255, 255, 255, 255));
            rect.setOutlineThickness(1);
            m_engine->window().draw(rect);
        }
    }
}

void Scene::renderCursor() {
    // TODO: Implement cursor rendering here
}

void Scene::renderHealth(std::shared_ptr<Entity> e) {
    if (!e->hasComponent<CHealth>() || !e->hasComponent<CAnimation>()) {
        return;
    }

    constexpr float y_offset{ 16.0f };
    constexpr float hbar_h{ 8.0f };
    auto animation_size = e->getComponent<CAnimation>().animation.getSize();
    auto pos = e->getComponent<CTransform>().pos;
    sf::RectangleShape hbox_back;
    sf::RectangleShape hbox_front;

    hbox_back.setFillColor(sf::Color(0, 0, 0));
    hbox_back.setSize(sf::Vector2f(animation_size.x, hbar_h));
    hbox_back.setPosition(pos.x - animation_size.x/2, pos.y - animation_size.y/2 - y_offset);
    hbox_front.setFillColor(sf::Color(255, 0, 0));
    hbox_front.setSize(sf::Vector2f(e->getComponent<CHealth>().percentage * animation_size.x, hbar_h));
    hbox_front.setPosition(pos.x - animation_size.x/2, pos.y - animation_size.y/2 - y_offset);

    m_engine->window().draw(hbox_back);
    m_engine->window().draw(hbox_front);
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

bool Scene::targetReached(const Vec2& pos, const Vec2& target) const {
    float distance = pos.distance(target);
    return fabs(distance) <= 5.0f;
}
