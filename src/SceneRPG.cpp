#include <string>
#include <fstream>

#include "Scene.h"
#include "GameEngine.h"
#include "Physics.h"
#include "math/Random.h"

SceneRPG::SceneRPG(GameEngine* engine, const std::string& level_path)
    : Scene(engine), m_level_path(level_path) {
    init(level_path);
}

// TODO: Init common actions in base class?
void SceneRPG::init(const std::string& level_path) {
    registerAction(sf::Keyboard::P, ActionName::PAUSE);
    registerAction(sf::Keyboard::Escape, ActionName::QUIT);
    registerAction(sf::Keyboard::T, ActionName::TOGGLE_TEXTURE);
    registerAction(sf::Keyboard::X, ActionName::TOGGLE_COLLISION);
    registerAction(sf::Keyboard::Y, ActionName::TOGGLE_FOLLOW);
    registerAction(sf::Keyboard::H, ActionName::TOGGLE_HEALTH);
    registerAction(sf::Keyboard::Q, ActionName::TOGGLE_AI_INFO);

    registerAction(sf::Keyboard::Space, ActionName::ATTACK);
    registerAction(sf::Keyboard::Up, ActionName::UP);
    registerAction(sf::Keyboard::Right, ActionName::RIGHT);
    registerAction(sf::Keyboard::Down, ActionName::DOWN);
    registerAction(sf::Keyboard::Left, ActionName::LEFT);

    registerAction(sf::Mouse::Button::Left, ActionName::LEFT_CLICK);
    registerAction(sf::Mouse::Button::Middle, ActionName::MIDDLE_CLICK);
    registerAction(sf::Mouse::Button::Right, ActionName::RIGHT_CLICK);

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

            Vec2 pos = getPosition(room_x, room_y, x, y);
            Tag tag = Tag::DEC;
            if (block_movement) {
                tag = Tag::TILE;
            } else if (animation == "Heart") {
                tag = Tag::HEART;
            } else if (animation == "Doorway") {
                tag = Tag::TELEPORT;
                m_doorways.push_back(pos);
            }

            auto tile = m_entity_manager.addEntity(tag);
            tile->addComponent<CAnimation>(m_engine->assets().getAnimation(animation), true);
            tile->addComponent<CTransform>(pos);
            tile->addComponent<CDraggable>();
            if (block_movement || block_vision) {
                const auto& animation_size = tile->getComponent<CAnimation>().animation.getSize();
                tile->addComponent<CBBox>(animation_size, block_movement, block_vision);
            }
        } else if (str == "Player") {
            float x, y, bbox_w, bbox_h, v;
            int health;
            file >> x >> y >> bbox_w >> bbox_h >> v >> health;
            m_player_config = {
                x, y, bbox_w, bbox_h, v, health
            };
        } else if (str == "NPC") { 
            std::string animation, mode;
            float room_x, room_y, x, y;
            bool block_movement, block_vision;
            int hp, damage;
            file >> animation >> room_x >> room_y >> x >> y >> block_movement >> block_vision >> hp >> damage;
            auto enemy = m_entity_manager.addEntity(Tag::ENEMY);
            enemy->addComponent<CAnimation>(m_engine->assets().getAnimation(animation), true);
            enemy->addComponent<CTransform>(getPosition(room_x, room_y, x, y));
            enemy->addComponent<CHealth>(hp);
            enemy->addComponent<CDamage>(damage);
            if (block_movement) {
                const auto& animation_size = enemy->getComponent<CAnimation>().animation.getSize();
                enemy->addComponent<CBBox>(animation_size, block_movement, block_vision);
            }

            file >> mode;
            if (mode == "Patrol") {
                float speed;
                int n_positions;
                std::vector<Vec2> positions;
                file >> speed >> n_positions;
                for (int i = 0; i < n_positions; i++) {
                    file >> x >> y;
                    positions.push_back(getPosition(room_x, room_y, x, y));
                }
                enemy->addComponent<CPatrol>(positions, speed);
            } else if (mode == "Follow") {
                float speed, y, x;
                file >> speed >> x >> y;
                enemy->addComponent<CFollowPlayer>(getPosition(room_x, room_y, x, y), speed);
            }
        } else {
            std::cerr << "Unknown level object: " << str << '\n';
        }
    }
    file.close();

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
    m_player->addComponent<CHealth>(m_player_config.health, m_player_config.health - 1);
    m_player->addComponent<CState>(State::RUN);
    m_player->addComponent<CWeapon>();
}

void SceneRPG::spawnSword(std::shared_ptr<Entity> entity) {
    if (!entity->hasComponent<CWeapon>()) {
        return;
    }
    if (entity->getComponent<CWeapon>().remaining_cooldown != 0) {
        return;
    }

    entity->getComponent<CState>().state = State::ATTACK;
    const auto pos = entity->getComponent<CTransform>().pos;
    const auto facing = entity->getComponent<CTransform>().facing;

    auto sword = m_entity_manager.addEntity(Tag::SWORD);
    sword->addComponent<CLifespan>(20);
    setSwordPos(sword, facing, pos);

    // Set  weapon cooldown
    auto& weapon = entity->getComponent<CWeapon>();
    weapon.remaining_cooldown = weapon.max_cooldown;
    weapon.current_weapon_id = sword->id();

    // Play sword sound
    m_engine->playSound("SoundSword");
}

void SceneRPG::setSwordPos(std::shared_ptr<Entity> sword, const Vec2& facing, const Vec2& pos) {
    std::string sword_animation = "";
    Vec2 scale(1.0f, 1.0f);

    if (facing == Vec2(0.0f, -1.0f)) {
        sword_animation = "SwordUp";
    } else if (facing == Vec2(0.0f, 1.0f)) {
        sword_animation = "SwordDown";
    } else if (facing == Vec2(1.0f, 0.0f)) {
        sword_animation = "SwordSide";
        scale.x = -1.0f;
    } else if (facing == Vec2(-1.0f, 0.0f)) {
        sword_animation = "SwordSide";
    }

    Vec2 swor_pos(
        pos.x + facing.x*m_grid_size.x,
        pos.y + facing.y*m_grid_size.y
    );

    Vec2 sword_bbox = facing.x == 0.0f ? Vec2(32.0f, 64.0f) : Vec2(64.0f, 32.0f);

    sword->addComponent<CAnimation>(m_engine->assets().getAnimation(sword_animation));
    sword->addComponent<CTransform>(swor_pos);
    sword->addComponent<CDamage>();
    sword->addComponent<CBBox>(sword_bbox, true, false);

    sword->getComponent<CTransform>().scale = scale;
}

void SceneRPG::teleport() {
    if (m_doorways.size() <= 0) {
        return;
    }
    Vec2 next_door = *math::getRandomElement(m_doorways.begin(), m_doorways.end());
    m_player->getComponent<CTransform>().pos = next_door;
}

void SceneRPG::update() {
    if (!m_paused) {
        m_entity_manager.update();

        sAI();
        sMovement();
        sStatus();
        sCollision();
        sAnimation();
        sCamera();
    }

    sRender();
    m_current_frame++;
}

void SceneRPG::sMovement() {
    auto input = m_player->getComponent<CInput>();
    auto& p_transform = m_player->getComponent<CTransform>();
    p_transform.prev_facing = p_transform.facing;
    Vec2 new_velocity(0.0f, 0.0f);

    if (input.up) {
        p_transform.scale = Vec2(fabsf(p_transform.scale.x), p_transform.scale.y);
        p_transform.facing = Vec2(0.0f, -1.0f);
        new_velocity.y -= m_player_config.v;
        new_velocity.x = 0.0f;
    }
    if (input.down) {
        p_transform.scale = Vec2(fabsf(p_transform.scale.x), p_transform.scale.y);
        p_transform.facing = Vec2(0.0f, 1.0f);
        new_velocity.y += m_player_config.v;
        new_velocity.x = 0.0f;
    }
    if (input.left) {
        p_transform.scale = Vec2(fabsf(p_transform.scale.x), p_transform.scale.y);
        p_transform.facing = Vec2(-1.0f, 0.0f);
        new_velocity.x -= m_player_config.v;
        new_velocity.y = 0.0f;
    }
    if (input.right) {
        p_transform.scale = Vec2(-fabsf(p_transform.scale.x), p_transform.scale.y);
        p_transform.facing = Vec2(1.0f, 0.0f);
        new_velocity.x += m_player_config.v;
        new_velocity.y = 0.0f;
    }
    p_transform.velocity = new_velocity;

    if (input.attack) {
        if (m_can_attack) {
            spawnSword(m_player);
        }
        m_can_attack = false;
    } else {
        m_can_attack = true;
    }

    for (auto e : m_entity_manager.getEntities()){
        auto& transform = e->getComponent<CTransform>();
        transform.prev_pos = transform.pos;
        transform.pos += transform.velocity;
        // transform.angle += 0.1f; // NOTE: For crazy effects

        if (e->hasComponent<CWeapon>()) {
            auto weapon = e->getComponent<CWeapon>();
            auto weapon_e = m_entity_manager.getEntity(weapon.current_weapon_id);
            if (weapon_e) {
                setSwordPos(weapon_e, transform.facing, transform.pos);
            }
        }
    }
}

void SceneRPG::sDoAction(const Action& action) {
    if (action.getType() == ActionType::START) {
        switch (action.getName()) {
            case ActionName::PAUSE: m_paused = !m_paused; break;
            case ActionName::QUIT: onEnd(); break;
            case ActionName::TOGGLE_FOLLOW: m_follow = !m_follow; break;
            case ActionName::TOGGLE_HEALTH: m_show_health = !m_show_health; break;
            case ActionName::TOGGLE_TEXTURE: m_draw_textures = !m_draw_textures; break;
            case ActionName::TOGGLE_COLLISION: m_draw_collision = !m_draw_collision; break;
            case ActionName::TOGGLE_AI_INFO: m_show_ai_info = !m_show_ai_info; break;
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

bool SceneRPG::targetReached(const Vec2& pos, const Vec2& target) const {
    float distance = pos.distance(target);
    return fabs(distance) <= 5.0f;
}

void SceneRPG::sAI() {
    for (auto e : m_entity_manager.getEntities(Tag::ENEMY)) {
        // Patrol
        if (e->hasComponent<CPatrol>()) {
            auto& patrol = e->getComponent<CPatrol>();
            auto& transform = e->getComponent<CTransform>();

            Vec2 target = patrol.positions[patrol.cur_pos];
            if (targetReached(transform.pos, target)) {
                patrol.cur_pos = patrol.cur_pos + 1 < patrol.positions.size() ? patrol.cur_pos + 1 : 0;
                target = patrol.positions[patrol.cur_pos];
            }

            Vec2 desired = target - transform.pos;
            desired = desired.normalize();
            desired = desired*patrol.speed;
            transform.velocity = desired;
        }

        // Follow
        if (e->hasComponent<CFollowPlayer>()) {
            Vec2 target = m_player->getComponent<CTransform>().pos;
            auto& transform = e->getComponent<CTransform>();
            e->getComponent<CFollowPlayer>().detected = true;

            for (auto tile : m_entity_manager.getEntities(Tag::TILE)) {
                if (!tile->hasComponent<CBBox>()) {
                    continue;
                }
                if (!tile->getComponent<CBBox>().block_vision) {
                    continue;
                }

                if (physics::entityIntersect(transform.pos, target, tile)) {
                    target = e->getComponent<CFollowPlayer>().home;
                    e->getComponent<CFollowPlayer>().detected = false;
                    break;
                }
            }

            if (!targetReached(transform.pos, target)) {
                Vec2 desired = target - transform.pos;
                desired = desired.normalize();
                desired = desired*e->getComponent<CFollowPlayer>().speed;
                transform.velocity = desired;
            } else {
                transform.velocity = Vec2(0.0f, 0.0f);
            }

            // TODO: implement steering:
            // Vec2 steering = desired - transform.velocity;
            // steering.scale(0.5f);
            // Vec2 actual = transform.velocity + steering;
        }
    }
}

void SceneRPG::sStatus() {
    // Invicibility frames
    if (m_player->hasComponent<CInvincibility>()) {
        if (m_player->getComponent<CInvincibility>().i_frames-- <= 0) {
            m_player->removeComponent<CInvincibility>();
        }
    }

    // Weapon cooldowns
    if (m_player->hasComponent<CWeapon>()) {
        auto& weapon = m_player->getComponent<CWeapon>();
        if (weapon.remaining_cooldown > 0) {
            weapon.remaining_cooldown--;
            if (weapon.remaining_cooldown == 0) {
                m_player->getComponent<CState>().state = State::RUN;
            }
        }
    }

    // Lifespans
    for (auto entity : m_entity_manager.getEntities()) {
        if (!entity->hasComponent<CLifespan>()) {
            continue;
        }

        auto& lifespan = entity->getComponent<CLifespan>();

        if (entity->tag() == Tag::SWORD && lifespan.remaining < lifespan.total) {
            entity->removeComponent<CDamage>();
        }

        if (lifespan.remaining-- <= 0) {
            entity->destroy();
        }
    }
}

void SceneRPG::sCollision() {
    auto& transfrom = m_player->getComponent<CTransform>();

    // Player - tile collision
    for (auto entity : m_entity_manager.getEntities(Tag::TILE)) {
        if (!entity->getComponent<CBBox>().block_movement) {
            continue;
        }
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

    // Player - enemy collision
    for (auto enemy : m_entity_manager.getEntities(Tag::ENEMY)) {
        auto enemy_damage = enemy->getComponent<CDamage>().damage;

        for (auto sword : m_entity_manager.getEntities(Tag::SWORD)) {
            if (physics::overlapping(enemy, sword)) {
                if (!sword->hasComponent<CDamage>()) {
                    continue;
                }
                auto& hp = enemy->getComponent<CHealth>();
                hp.current -= sword->getComponent<CDamage>().damage;
                hp.percentage = static_cast<float>(hp.current)/static_cast<float>(hp.max);

                if (hp.current <= 0) {
                    enemy->destroy();
                }
            }
        }

        if (m_player->hasComponent<CInvincibility>()) {
            continue;
        }

        if (physics::overlapping(m_player, enemy)) {
            auto& hp = m_player->getComponent<CHealth>();
            hp.current -= enemy_damage;
            hp.percentage = static_cast<float>(hp.current)/static_cast<float>(hp.max);

            if (hp.current <= 0) {
                m_player->destroy();
                spawnPlayer();
            } else {
                m_player->addComponent<CInvincibility>();
            }
        }
    }

    // Player - heart collision
    for (auto heart : m_entity_manager.getEntities(Tag::HEART)) {
        if (physics::overlapping(m_player, heart)) {
            auto& p_health = m_player->getComponent<CHealth>();
            p_health.current = std::min(p_health.current + 1, p_health.max);
            p_health.percentage = static_cast<float>(p_health.current)/static_cast<float>(p_health.max);
            heart->destroy();
        }
    }

    // Player - doorway/teleport collision
    for (auto doorway : m_entity_manager.getEntities(Tag::TELEPORT)) {
        if (physics::overlapping(m_player, doorway) && !physics::previouslyOverlapping(m_player, doorway)) {
            teleport();
        }
    }
}

void SceneRPG::sAnimation() {
    for (auto entity : m_entity_manager.getEntities()) {
        if (!entity->hasComponent<CAnimation>()) {
            continue;
        }

        if (entity->tag() == Tag::PLAYER) {
            auto& p_state = m_player->getComponent<CState>();
            auto p_transform = m_player->getComponent<CTransform>();
            if (p_transform.facing != p_transform.prev_facing || p_state.state != p_state.prev_state) {
                switch (p_state.state) {
                    case State::RUN:
                        if (p_transform.facing == Vec2(0.0f, -1.0f)) {
                            m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PUp"), true); break;
                        } else if (p_transform.facing == Vec2(0.0f, 1.0f)) {
                            m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PDown"), true); break;
                        } else if (p_transform.facing == Vec2(1.0f, 0.0f)) {
                            m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PSide"), true); break;
                        } else if (p_transform.facing == Vec2(-1.0f, 0.0f)) {
                            m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PSide"), true); break;
                        }
                        break;
                    case State::ATTACK:
                        if (p_transform.facing == Vec2(0.0f, -1.0f)) {
                            m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PAttackUp"), true); break;
                        } else if (p_transform.facing == Vec2(0.0f, 1.0f)) {
                            m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PAttackDown"), true); break;
                        } else if (p_transform.facing == Vec2(1.0f, 0.0f)) {
                            m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PAttackSide"), true); break;
                        } else if (p_transform.facing == Vec2(-1.0f, 0.0f)) {
                            m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PAttackSide"), true); break;
                        }
                        break;
                    default:
                        break;
                }
            }
            p_state.prev_state = p_state.state;

            if (m_player->getComponent<CTransform>().velocity != Vec2(0.0f, 0.0f)) {
                entity->getComponent<CAnimation>().animation.update();
            }

            if (m_player->hasComponent<CInvincibility>()) {
		        entity->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 128, 128, 128));
	        } else {
                entity->getComponent<CAnimation>().animation.getSprite().setColor(sf::Color(255, 255, 255));
            }
        } else {
            entity->getComponent<CAnimation>().animation.update();
        }

        if (!entity->getComponent<CAnimation>().repeat && entity->getComponent<CAnimation>().animation.hasEnded()) {
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
    if (m_draw_textures) {
        for (auto e : m_entity_manager.getEntities()) {
            auto& transform = e->getComponent<CTransform>();
            if (e->hasComponent<CAnimation>()) {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                m_engine->window().draw(animation.getSprite());

                if (m_show_health) {
                    renderHealth(e);
                }

	            if (m_show_ai_info && e->tag() == Tag::ENEMY) {
                    renderInfoAI(e, m_player);
                }
            }
        }
    }

    if (m_draw_collision) {
        renderBBoxes();
    }
}

void SceneRPG::onEnd() {
    // Reset view
    m_engine->window().setView(m_engine->window().getDefaultView());

    // Go back to menu
    m_engine->changeScene(SceneType::MENU, std::make_shared<SceneMenu>(m_engine), true);
}
