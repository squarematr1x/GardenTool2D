#pragma once

#include <SFML/System/Clock.hpp>

#include "../ecs/entity.hpp"

enum class Mode { // NOTE: could be used for UI
    ADD,
    EDIT
};

class Editor {
    std::string m_level_path{ "" }; // Current level
    sf::Clock m_dt;

public:
    ~Editor();

    void init (sf::RenderWindow& window);
    void render(sf::RenderWindow& window);
    void processEvent(const sf::RenderWindow& window, const sf::Event& event);

    void addEntity(std::shared_ptr<Entity> e);
    void modifyEntity(std::shared_ptr<Entity> e);
    void deleteEntity(std::shared_ptr<Entity> e);

    void setLevel(const std::string& level_path) { m_level_path = level_path; }
};
