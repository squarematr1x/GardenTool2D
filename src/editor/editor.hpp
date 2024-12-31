#pragma once

#include <SFML/System/Clock.hpp>

#include "../ecs/entity.hpp"

struct PlayerConfig;

class EntityManager;
class GameEngine;

class Editor {
    const char* m_types[6] = {
        "TILE",
        "ENEMY",
        "HEART",
        "ELEVATOR",
        "TELEPORT",
        "CHECKPOINT"
    };

    const std::map<std::string, Tag> m_type_map = {
        { "TILE", Tag::TILE },
        { "ENEMY", Tag::ENEMY },
        { "HEART", Tag::HEART },
        { "ELEVATOR", Tag::ELEVATOR },
        { "TELEPORT", Tag::TELEPORT },
        { "CHECKPOINT", Tag::CHECKPOINT }
    };

    std::string m_level_path{ "" }; // Current level
    sf::Clock m_dt;
    bool m_previously_created{ false };
    std::string m_previous_animation{ "Brick" };
    std::vector<std::string> m_level_content;

    void addEntity(EntityManager& entity_manager, GameEngine* engine);
    void parseEntity(std::shared_ptr<Entity> e, GameEngine* engine);
    void parseEntities(EntityManager& entity_manager, GameEngine* engine);

    int editTabFlag();

    const std::string tagString(Tag tag) const;

public:
    ~Editor();

    void init (sf::RenderWindow& window);
    void update(sf::RenderWindow& window, EntityManager& entity_manager, GameEngine* engine);
    void processEvent(const sf::RenderWindow& window, const sf::Event& event);

    void setLevel(const std::string& level_path);

    bool windowActive() const;
};
