#include "Scene.h"
#include "GameEngine.h"

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
