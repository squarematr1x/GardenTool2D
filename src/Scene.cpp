#include "Scene.h"
#include "GameEngine.h"

void Scene::drawLine(const Vec2& p1, const Vec2& p2) {
    sf::Vertex line[] = {
        {{p1.x, p1.y}, sf::Color(255, 255, 255)},
        {{p2.x, p2.y}, sf::Color(255, 255, 255)}
    };
    m_game_engine->window().draw(line, 2, sf::Lines);
}

size_t Scene::width() const {
    return 0;
}

size_t Scene::height() const {
    return 0;
}
