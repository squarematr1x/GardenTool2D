#include "Scene.h"
#include "GameEngine.h"

size_t Scene::width() const {
    return m_engine->window().getSize().x;
}

size_t Scene::height() const {
    return m_engine->window().getSize().y;
}

void Scene::drawLine(const Vec2& p1, const Vec2& p2) {
    sf::Vertex line[] = {
        {{p1.x, p1.y}, sf::Color(255, 255, 255)},
        {{p2.x, p2.y}, sf::Color(255, 255, 255)}
    };
    m_engine->window().draw(line, 2, sf::Lines);
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
