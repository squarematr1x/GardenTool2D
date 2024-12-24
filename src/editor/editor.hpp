#pragma once

#include <SFML/System/Clock.hpp>

#include "../ecs/entity.hpp"

struct PlayerConfig;

class EntityManager;
class GameEngine;

enum class Tag : unsigned char;

class Editor {
    struct EntityConfig {
        Vec2 pos{ 0, 0 }; // NOTE: this should be actually just mouse pos and let the user drop the entity to its final pos
        Vec2 bbox{ 0, 0 };
        bool block_movement{ false };
        bool block_vision{ false };
        char animation_name[128]{ "" };
        Tag tag{ Tag::DEFAULT };
        
        // Other stuff such as gravity, damage, etc.
        // First implement adding of tiles
    };

    const char* m_types[11] = {
        "TILE",
        "PLAYER",
        "ENEMY",
        "BULLET",
        "EXPLOSION",
        "SWORD",
        "HEART",
        "TELEPORT",
        "ELEVATOR",
        "CHECKPOINT"
    };

    std::string m_level_path{ "" }; // Current level
    sf::Clock m_dt;
    EntityConfig m_entity_config;
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
