#include <string>
#include <fstream>

#include "Scene.h"
#include "GameEngine.h"

SceneRPG::SceneRPG(GameEngine* engine, const std::string& level_path)
    : Scene(engine), m_level_path(level_path) {
    init(level_path);
}

void SceneRPG::init(const std::string& level_path) {
    registerAction(sf::Keyboard::P, ActionName::PAUSE);
    registerAction(sf::Keyboard::Escape, ActionName::QUIT);
    registerAction(sf::Keyboard::T, ActionName::TOGGLE_TEXTURE);
    registerAction(sf::Keyboard::C, ActionName::TOGGLE_COLLISION);
    registerAction(sf::Keyboard::Y, ActionName::TOGGLE_FOLLOW);

    registerAction(sf::Keyboard::Space, ActionName::UP);
    registerAction(sf::Keyboard::Up, ActionName::UP);
    registerAction(sf::Keyboard::Right, ActionName::RIGHT);
    registerAction(sf::Keyboard::Down, ActionName::DOWN);
    registerAction(sf::Keyboard::Left, ActionName::LEFT);
    registerAction(sf::Keyboard::Z, ActionName::SHOOT);

    registerAction(sf::Mouse::Button::Left, ActionName::LEFT_CLICK);
    registerAction(sf::Mouse::Button::Middle, ActionName::MIDDLE_CLICK);
    registerAction(sf::Mouse::Button::Right, ActionName::RIGHT_CLICK);

    // m_grid_text.setCharacterSize(12);
    // m_grid_text.setFont(m_engine->assets().getFont("Arial"));
    // m_pause_text.setCharacterSize(20);
    // m_pause_text.setFont(m_engine->assets().getFont("Arial"));

    loadLevel(level_path);
}

void SceneRPG::loadLevel(const std::string& path) {
    m_entity_manager = EntityManager();

    std::ifstream file(path);
    std::string str;

    while (file.good()) {
        file >> str;

        if (str == "Tile") {
            std::string animation;
            float room_x, room_y, x, y;
            bool block_movement, block_vision;
            file >> animation >> room_x >> room_y >> x >> y >> block_movement >> block_vision;

            auto tile = block_movement ? m_entity_manager.addEntity(Tag::TILE) : m_entity_manager.addEntity(Tag::DEC);
            tile->addComponent<CAnimation>(m_engine->assets().getAnimation(animation), true);
            tile->addComponent<CTransform>(getPosition(room_x, room_y, x, y));
            tile->addComponent<CDraggable>();
            if (block_movement) {
                const auto& animation_size = tile->getComponent<CAnimation>().animation.getSize();
                tile->addComponent<CBBox>(animation_size, block_movement, block_vision);
                m_entity_manager.addEntity(Tag::TILE);
            }
        } else if (str == "Player") {
            float x, y, bbox_w, bbox_h, v;
            int health;
            file >> x >> y >> bbox_w >> bbox_h >> v >> health;
            m_player_config = {
                x, y, bbox_w, bbox_h, v, health
            };
        } else {
            std::cerr << "Unknown level object: " << str << '\n';
        }
    }

    spawnPlayer();
}

Vec2 SceneRPG::getPosition(float rx, float ry, float tx, float ty) const {
    const float room_start_x = rx * m_grid_size.x * m_room_size.x;
    const float room_start_y = ry * m_grid_size.y * m_room_size.y;

    return Vec2(room_start_x + tx*m_grid_size.x, room_start_y + ty*m_grid_size.y);
}

void SceneRPG::spawnPlayer() {
    m_player = m_entity_manager.addEntity(Tag::PLAYER);
    m_player->addComponent<CTransform>(Vec2(m_player_config.x*m_grid_size.x, m_player_config.y*m_grid_size.y));
    m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PDown"), true);
    m_player->addComponent<CBBox>(Vec2(m_player_config.bbox_x, m_player_config.bbox_y), true, false);
    m_player->addComponent<CHealth>(m_player_config.health, m_player_config.health);
}

void SceneRPG::spawnSword(std::shared_ptr<Entity> entity) {
    (void)entity;
    // CTransform facing example: left: (-1, 0), up: (0, -1), right: (1, 0), down (0, 1)
	// facing can be used to calculate offset of a weapon from player pos
	// e.g. spawn sword pos = (ppos.x + facing.x*grid_x, ppos.y +facing.y*grid_y)

    // Sword should appropriate lifespan, location based on player's facing dir, damage value of 1
    // and play "Slash" sound
}

void SceneRPG::update() {
    m_entity_manager.update();
    // TODO: Implement pause

    sAI();
    sMovement();
    sStatus();
    sCollision();
    sAnimation();
    // sCamera();
    sRender();

    m_current_frame++;
}

void SceneRPG::sMovement() {
    // TODO: Implement player movement functionality based on input
    auto input = m_player->getComponent<CInput>();
    auto& state = m_player->getComponent<CState>();
    auto& scale = m_player->getComponent<CTransform>().scale;
    if (input.up) { state = State::UP; }
    if (input.down) { state = State::DOWN; }
    if (input.left) {
        scale = Vec2(fabsf(scale.x), scale.y);
        state = State::LEFT; 
    }
    if (input.right) {
        scale = Vec2(-fabsf(scale.x), scale.y);
        state = State::RIGHT;
    }
}

void SceneRPG::sDoAction(const Action& action) {
    // TODO: Setting of the player's input component variables only
    //       use minimalistic logic

    if (action.getType() == ActionType::START) {
        switch (action.getName()) {
            case ActionName::PAUSE: break;
            case ActionName::QUIT: onEnd(); break;
            case ActionName::TOGGLE_FOLLOW: m_follow = !m_follow; break;
            case ActionName::TOGGLE_TEXTURE: break;
            case ActionName::TOGGLE_COLLISION: break;
            case ActionName::UP: m_player->getComponent<CInput>().up = true; break;
            case ActionName::RIGHT: m_player->getComponent<CInput>().right = true; break;
            case ActionName::DOWN: m_player->getComponent<CInput>().down = true; break;
            case ActionName::LEFT: m_player->getComponent<CInput>().left = true; break;
            default: break;
        }
    } else if (action.getType() == ActionType::END) {
        switch (action.getName()) {
            case ActionName::UP: m_player->getComponent<CInput>().up = false; break;
            case ActionName::DOWN: m_player->getComponent<CInput>().down = false; break;
            case ActionName::RIGHT: m_player->getComponent<CInput>().right = false; break;
            case ActionName::LEFT: m_player->getComponent<CInput>().left = false; break;
            default: break;
        }
    }
}

void SceneRPG::sAI() {
    // Implement follow and patrol behavior
}

void SceneRPG::sStatus() {
    // Implement lifespan and invicibility frames here
}

void SceneRPG::sCollision() {
    // Implement entity - tile collision
    // Implement player - enemy collision with damage calculations
    // Implement sword - enemy collisions
    // Implement black tile collision/teleporting
    // Implement entity -heart collisions and life gain logic
    // Implement util functions for beforementioned logic when needed
}

void SceneRPG::sAnimation() {
    // Implement player facing direction animation
    // Implement sword animation based on player direction (sword should also move if the player changes direction mid swing)
    // Implement destruction of entities with non-repeating finished animations
    for (auto entity : m_entity_manager.getEntities()) {
        if (!entity->hasComponent<CAnimation>()) { continue; }

        entity->getComponent<CAnimation>().animation.update();

        if (!entity->getComponent<CAnimation>().repeat &&
            entity->getComponent<CAnimation>().animation.hasEnded()) {
            entity->destroy();
        }

        if (entity->tag() == Tag::PLAYER) {
            auto& p_state = m_player->getComponent<CState>();
            if (p_state.state != p_state.prev_state) {
                switch (p_state.state) {
                    case State::UP:
                        m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PUp"), true); break;
                    case State::DOWN:
                        m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PDown"), true); break;
                    case State::LEFT:
                        m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PSide"), true); break;
                    case State::RIGHT:
                        m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PSide"), true); break;
                    default: break;
                }
            }
            p_state.prev_state = p_state.state;
        }
    }
}

void SceneRPG::sCamera() {
    // Implement camera view logic

    // get the current view, which we will modify in the if-statement below
    sf::View view = m_engine->window().getView();

    if (m_follow) {
        // Get view from player follow camera
    } else {
        // Get view for room-based camera
    }
    m_engine->window().setView(view);
}

void SceneRPG::sRender() {
    m_engine->window().clear(sf::Color(215, 189, 164));

    // draw all Entity textures/animations
    for (auto e : m_entity_manager.getEntities()) {
        auto& transform = e->getComponent<CTransform>();
        if (e->hasComponent<CAnimation>()) {
            auto& animation = e->getComponent<CAnimation>().animation;
            animation.getSprite().setRotation(transform.angle);
            animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
            animation.getSprite().setScale(transform.scale.x, transform.scale.y);
            m_engine->window().draw(animation.getSprite());
        }
    }
}

void SceneRPG::onEnd() {
    // Reset view
    m_engine->window().setView(m_engine->window().getDefaultView());

    // Go back to menu
    m_engine->changeScene(SceneType::MENU, std::make_shared<SceneMenu>(m_engine), true);
}
