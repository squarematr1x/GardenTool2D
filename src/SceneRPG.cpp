#include <string>
#include <fstream>

#include "Scene.h"
#include "GameEngine.h"
#include "Physics.h"

SceneRPG::SceneRPG(GameEngine* engine, const std::string& level_path)
    : Scene(engine), m_level_path(level_path) {
    init(level_path);
}

// TODO: Init common actions in base class?
void SceneRPG::init(const std::string& level_path) {
    registerAction(sf::Keyboard::P, ActionName::PAUSE);
    registerAction(sf::Keyboard::Escape, ActionName::QUIT);
    registerAction(sf::Keyboard::T, ActionName::TOGGLE_TEXTURE);
    registerAction(sf::Keyboard::C, ActionName::TOGGLE_COLLISION);
    registerAction(sf::Keyboard::Y, ActionName::TOGGLE_FOLLOW);

    registerAction(sf::Keyboard::Space, ActionName::ATTACK);
    registerAction(sf::Keyboard::Up, ActionName::UP);
    registerAction(sf::Keyboard::Right, ActionName::RIGHT);
    registerAction(sf::Keyboard::Down, ActionName::DOWN);
    registerAction(sf::Keyboard::Left, ActionName::LEFT);

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

Vec2 SceneRPG::getCurrentRoom() const {
    Vec2 p_pos = m_player->getComponent<CTransform>().pos;
    return Vec2(floorf(p_pos.x/(m_room_size.x*m_grid_size.x)), floorf(p_pos.y/(m_room_size.y*m_grid_size.y)));
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
    sCamera();
    sRender();

    m_current_frame++;
}

void SceneRPG::sMovement() {
    auto input = m_player->getComponent<CInput>();
    auto& p_state = m_player->getComponent<CState>().state;
    auto& p_transform = m_player->getComponent<CTransform>();
    Vec2 new_velocity(0.0f, 0.0f);

    if (input.up) {
        p_state = State::UP;
        new_velocity.y -= m_player_config.v;
    }
    if (input.down) {
        p_state = State::DOWN;
        new_velocity.y += m_player_config.v;
    }
    if (input.left) {
        p_state = State::LEFT;
        p_transform.scale = Vec2(fabsf(p_transform.scale.x), p_transform.scale.y);
        new_velocity.x -= m_player_config.v;
    }
    if (input.right) {
        p_state = State::RIGHT;
        p_transform.scale = Vec2(-fabsf(p_transform.scale.x), p_transform.scale.y);
        new_velocity.x += m_player_config.v;
    }
    
    if (new_velocity.x == 0.0f && new_velocity.y == 0.0f) {
        p_state = State::STAND;
    }
    p_transform.velocity = new_velocity;

    for (auto e : m_entity_manager.getEntities()){
        auto& transform = e->getComponent<CTransform>();
        transform.prev_pos = transform.pos;
        transform.pos += transform.velocity;
    }
}

void SceneRPG::sDoAction(const Action& action) {
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
            case ActionName::ATTACK: m_player->getComponent<CInput>().attack = true; break;
            default: break;
        }
    } else if (action.getType() == ActionType::END) {
        switch (action.getName()) {
            case ActionName::UP: m_player->getComponent<CInput>().up = false; break;
            case ActionName::DOWN: m_player->getComponent<CInput>().down = false; break;
            case ActionName::RIGHT: m_player->getComponent<CInput>().right = false; break;
            case ActionName::LEFT: m_player->getComponent<CInput>().left = false; break;
            case ActionName::ATTACK: m_player->getComponent<CInput>().attack = false; break;
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
    // Implement player - enemy collision with damage calculations
    // Implement sword - enemy collisions
    // Implement black tile collision/teleporting
    // Implement entity -heart collisions and life gain logic
    // Implement util functions for beforementioned logic when needed
    auto& transfrom = m_player->getComponent<CTransform>();

    for (auto entity : m_entity_manager.getEntities(Tag::TILE)) {
        if (!entity->getComponent<CBBox>().block_movement) {
            continue;
        }
        // Player - tile collision
        Vec2 overlap = physics::getOverlap(m_player, entity);
        if (overlap.x > 0 && overlap.y > 0) {
            Vec2 prev_overlap = physics::getPrevOverlap(m_player, entity);
            if (prev_overlap.y > 0) {
                if (transfrom.velocity.x > 0) {
                    transfrom.pos.x -= overlap.x;
                } else if (transfrom.velocity.x < 0) {
                    transfrom.pos.x += overlap.x;
                }
                transfrom.velocity.x = 0.0f;
            }
            if (prev_overlap.x > 0) {
                if (transfrom.velocity.y > 0) {
                    transfrom.pos.y -= overlap.y;
                } else if (transfrom.velocity.y < 0) {
                    transfrom.pos.y += overlap.y;
                }
                transfrom.velocity.y = 0.0f;
            }
        }
    }
}

void SceneRPG::sAnimation() {
    // Implement sword animation based on player direction (sword should also move if the player changes direction mid swing)
    // Implement destruction of entities with non-repeating finished animations
    for (auto entity : m_entity_manager.getEntities()) {
        if (!entity->hasComponent<CAnimation>()) { continue; }

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

            if (p_state.state != State::STAND) {
                entity->getComponent<CAnimation>().animation.update();
            }
        } else {
            entity->getComponent<CAnimation>().animation.update();
        }

        if (!entity->getComponent<CAnimation>().repeat &&
            entity->getComponent<CAnimation>().animation.hasEnded()) {
            entity->destroy();
        }
    }
}

void SceneRPG::sCamera() {
    sf::View view = m_engine->window().getView();
    if (m_follow) {
        // Get view from player follow camera
        auto& p_pos = m_player->getComponent<CTransform>().pos;
        view.setCenter(p_pos.x, p_pos.y);
    } else {
        // Get view for room-based camera
        auto window_size = m_engine->window().getSize();
        Vec2 room = getCurrentRoom();
        view.setCenter(
            static_cast<float>((window_size.x/2) + room.x*window_size.x),
            static_cast<float>((window_size.y/2) + room.y*window_size.y)
        );
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
