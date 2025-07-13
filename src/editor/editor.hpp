#pragma once

#include <map>

#include <SFML/System/Clock.hpp>

#include "../ecs/entity.hpp"

struct PlayerConfig;

class EntityManager;
class GameEngine;

// NOTE: Editor is only a dev tool, thus never seen/used by the end user
class Editor {
    const char* m_types[8] = {
        "TILE",
        "ENEMY",
        "HEART",
        "ELEVATOR",
        "TELEPORT",
        "CHECKPOINT",
        "TRIGGER",
        "TRIGGERABLE"
    };

    const std::map<std::string, Tag> m_type_map = {
        { "TILE", Tag::TILE },
        { "ENEMY", Tag::ENEMY },
        { "HEART", Tag::HEART },
        { "ELEVATOR", Tag::ELEVATOR },
        { "TELEPORT", Tag::TELEPORT },
        { "CHECKPOINT", Tag::CHECKPOINT },
        { "TRIGGER", Tag::TRIGGER },
        { "TRIGGERABLE", Tag::TRIGGERABLE }
    };

    const char* m_trigger_types[3] = {
        "APPLY_GRAVITY",
        "DESTROY",
        "PLAY_MUSIC"
    };

    const std::map<std::string, TriggerType> m_trigger_type_map = {
        { "APPLY_GRAVITY", TriggerType::APPLY_GRAVITY },
        { "DESTROY", TriggerType::DESTROY },
        { "PLAY_MUSIC", TriggerType::PLAY_MUSIC }
    };

    // TODO: clean this
    const std::map<TriggerType, std::string> m_trigger_level_file_type_map = {
        { TriggerType::APPLY_GRAVITY, "ApplyGravity" },
        { TriggerType::DESTROY, "Destroy" },
        { TriggerType::PLAY_MUSIC, "PlayMusic" }
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
    
    int getNextTriggerId(EntityManager& entity_manager) const;

    std::vector<int> getTriggerIds(EntityManager& entity_manager) const;

    const std::string tagString(Tag tag) const;

public:
    ~Editor();

    void init (sf::RenderWindow& window);
    void update(sf::RenderWindow& window, EntityManager& entity_manager, GameEngine* engine);
    void processEvent(const sf::RenderWindow& window, const sf::Event& event);

    void setLevel(const std::string& level_path);

    bool windowActive() const;
};
