#include <string>
#include <fstream>
#include <sstream>

#include "scene.hpp"
#include "../engine.hpp"
#include "../collision/physics.hpp"
#include "../math/random.hpp"
#include "../pathfinding.hpp"
#include "../util/timer.hpp"
#include "../core/key.hpp"
#include "../core/mouse.hpp"

SceneRPG::SceneRPG(GameEngine* engine, const std::string& level_path)
    : Scene(engine, level_path) {
    init(level_path);
}

// TODO: Init common actions in base class?
void SceneRPG::init(const std::string& level_path) {
    registerAction(P, ActionName::PAUSE);
    registerAction(Escape, ActionName::QUIT);
    registerAction(T, ActionName::TOGGLE_TEXTURE);
    registerAction(X, ActionName::TOGGLE_COLLISION);
    registerAction(G, ActionName::TOGGLE_GRID);
    registerAction(Y, ActionName::TOGGLE_FOLLOW);
    registerAction(H, ActionName::TOGGLE_HEALTH);
    registerAction(Q, ActionName::TOGGLE_AI_INFO);
    registerAction(Tab, ActionName::TOGGLE_LEVEL_EDITOR);
    registerAction(LSystem, ActionName::L_SYSTEM);

    registerAction(Space, ActionName::ATTACK);
    registerAction(Up, ActionName::UP);
    registerAction(Right, ActionName::RIGHT);
    registerAction(Down, ActionName::DOWN);
    registerAction(Left, ActionName::LEFT);

    registerAction(mouse::Button::Left, ActionName::LEFT_CLICK);
    registerAction(mouse::Button::Middle, ActionName::MIDDLE_CLICK);
    registerAction(mouse::Button::Right, ActionName::RIGHT_CLICK);
    registerAction(mouse::Wheel::VerticalWheel, ActionName::MOUSE_SCROLL);

    loadLevel(level_path);

    setRoomSize();
}

void SceneRPG::loadLevel(const std::string& path) {
    // Reset the entity manager every time we load a level
    m_entity_manager = EntityManager();
    std::ifstream file(path);
    std::string line;

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            // Skip comments and empty lines
            continue;
        }

        std::string asset_type;
        std::istringstream text_stream(line);

        while (text_stream >> asset_type) {
            if (asset_type == "Tile") {
                std::string animation;
                float x, y;
                bool block_movement, block_vision;
                text_stream >> animation >> x >> y >> block_movement >> block_vision;

                Tag tag = Tag::TILE;
                if (animation == "Heart") {
                    tag = Tag::HEART;
                } else if (animation == "Doorway") {
                    tag = Tag::TELEPORT;
                }

                auto tile = m_entity_manager.addEntity(tag);
                tile->addComponent<CAnimation>(m_engine->assets().getAnimation(animation), true);
                if (block_movement || block_vision) {
                    const auto& animation_size = tile->getComponent<CAnimation>().animation.getSize();
                    tile->addComponent<CBBox>(animation_size, block_movement, block_vision);
                }
                auto pos = gridToMidPixel(x, y, tile);
                tile->addComponent<CTransform>(pos);
                if (tag == Tag::TELEPORT) {
                    m_doorways.push_back(pos);
                }
            } else if (asset_type == "Player") {
                float x, y, bbox_w, bbox_h, v, max_v, jump_v, gravity;
                int hp;
                std::string weapon = "Empty";
                text_stream >> x >> y >> bbox_w >> bbox_h >> v >> max_v >> jump_v >> gravity >> weapon >> hp;
                m_player_config = {
                    x, y, bbox_w, bbox_h, v, 0.0f, 0.0f, 0.0f, weapon, hp
                };
            } else if (asset_type == "NPC") { 
                std::string animation, mode;
                float x, y;
                bool block_movement, block_vision, hostile;
                int hp, damage;
                text_stream >> animation >> x >> y >> block_movement >> block_vision >> hostile >> hp >> damage;
                auto enemy = m_entity_manager.addEntity(Tag::ENEMY);
                enemy->addComponent<CAnimation>(m_engine->assets().getAnimation(animation), true);
                enemy->addComponent<CHealth>(hp);
                enemy->addComponent<CDamage>(damage);
                const auto& animation_size = enemy->getComponent<CAnimation>().animation.getSize();
                enemy->addComponent<CBBox>(animation_size, block_movement, block_vision);
                auto pos = gridToMidPixel(x, y, enemy);
                enemy->addComponent<CTransform>(pos, true);
                enemy->addComponent<CBehavior>(hostile);
                enemy->addComponent<CInteractable>();

                text_stream >> mode;
                if (mode == "Patrol") {
                    float speed;
                    int n_positions;
                    std::vector<Vec2> positions;
                    text_stream >> speed >> n_positions;
                    for (int i = 0; i < n_positions; i++) {
                        text_stream >> x >> y;
                        positions.push_back(getPosition(x, y));
                    }
                    enemy->addComponent<CPatrol>(positions, speed);
                } else if (mode == "Follow") {
                    float speed, y, x;
                    text_stream >> speed >> x >> y;
                    enemy->addComponent<CFollowPlayer>(getPosition(x, y), speed);
                }
            } else {
                std::cerr << "Unknown level object: " << asset_type << '\n';
            }
        }
    }
    file.close();

    spawnPlayer();
}

// TODO: Add gridToMidPixel here
Vec2 SceneRPG::getPosition(float x, float y) const {
    return Vec2(x*m_grid_cell_size.x + m_grid_cell_size.x/2, y*m_grid_cell_size.y + m_grid_cell_size.y/2);
}

// TODO: This can stay, but won't be used for now
Vec2 SceneRPG::getPosition(float rx, float ry, float tx, float ty) const {
    const auto room_start_x = rx*m_grid_cell_size.x*m_room_size.x;
    const auto room_start_y = ry*m_grid_cell_size.y*m_room_size.y;

    return Vec2(room_start_x + tx*m_grid_cell_size.x + m_grid_cell_size.x/2, room_start_y + ty*m_grid_cell_size.y + m_grid_cell_size.y/2);
}

Vec2 SceneRPG::getCurrentRoom() const {
    const auto p_pos = m_player->getComponent<CTransform>().pos;
    return Vec2(floorf(p_pos.x/(m_room_size.x*m_grid_cell_size.x)), floorf(p_pos.y/(m_room_size.y*m_grid_cell_size.y)));
}

void SceneRPG::spawnPlayer() {
    m_player = m_entity_manager.addEntity(Tag::PLAYER);
    m_player->addComponent<CTransform>(
        Vec2(
            m_player_config.x*m_grid_cell_size.x + m_grid_cell_size.x/2,
            m_player_config.y*m_grid_cell_size.y + m_grid_cell_size.y/2),
        true
    );
    m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("PDown"), true);
    m_player->addComponent<CBBox>(Vec2(m_player_config.bbox_x, m_player_config.bbox_y), true, false);
    m_player->addComponent<CHealth>(m_player_config.hp, m_player_config.hp);
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

    // Set weapon cooldown
    auto& weapon = entity->getComponent<CWeapon>();
    weapon.remaining_cooldown = weapon.max_cooldown;
    weapon.current_weapon_id = sword->id();

    // Play sword sound
    m_engine->playSound("SoundSword");
}

void SceneRPG::setSwordPos(std::shared_ptr<Entity> sword, const Vec2& facing, const Vec2& pos) {
    auto sword_animation = "";
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

    const Vec2 swor_pos(
        pos.x + facing.x*m_grid_cell_size.x,
        pos.y + facing.y*m_grid_cell_size.y
    );

    const auto sword_bbox = facing.x == 0.0f ? Vec2(32.0f, 64.0f) : Vec2(64.0f, 32.0f);

    sword->addComponent<CAnimation>(m_engine->assets().getAnimation(sword_animation));
    sword->addComponent<CTransform>(swor_pos, true);
    sword->addComponent<CDamage>();
    sword->addComponent<CBBox>(sword_bbox, true, false);

    sword->getComponent<CTransform>().scale = scale;
}

void SceneRPG::setRoomSize() {
    m_room_size = Vec2(width()/m_grid_cell_size.x, height()/m_grid_cell_size.y);
}

void SceneRPG::setFacing(std::shared_ptr<Entity> entity) {
    auto& transform = entity->getComponent<CTransform>();
    if (transform.velocity == Vec2(0.0f, 0.0f)) {
        return;
    }
    transform.facing = Vec2(0.0f, 0.0f);
    transform.scale = Vec2(fabsf(transform.scale.x), transform.scale.y);
    if (std::floor(transform.velocity.x) > 0.0f) {
        transform.facing.x = 1.0f;
        transform.scale = Vec2(-fabsf(transform.scale.x), transform.scale.y);
    } else if (std::ceil(transform.velocity.x) < 0.0f) {
        transform.facing.x = -1.0f;
    }
    if (std::floor(transform.velocity.y) > 0.0f) {
        transform.facing.y = 1.0f;
    } else if (std::ceil(transform.velocity.y) < 0.0f) {
        transform.facing.y = -1.0f;
    }
}

void SceneRPG::teleport(const Vec2& cur_doorway) {
    if (m_doorways.size() == 0) {
        return;
    }

    // Don't include the doorway the player just entered in the list
    std::vector<Vec2> reduced_doorways;
    for (const auto& doorway : m_doorways) {
        if (doorway != cur_doorway) {
            reduced_doorways.push_back(doorway);
        }
    }

    if (reduced_doorways.size() == 0) {
        return;
    }

    Vec2 next_door = *math::getRandomElement(reduced_doorways.begin(), reduced_doorways.end());
    m_player->getComponent<CTransform>().pos = next_door;
}

void SceneRPG::update() {
    if (!m_paused) {
        m_entity_manager.update();

        sAI();
        sMovement();
        sStatus();
        sInteract();
        sCollision();
        sAnimation();
    }

    sCamera();
    sDragAndDrop();
    sRender();
    m_current_frame++;
}

void SceneRPG::sMovement() {
    auto& p_transform = m_player->getComponent<CTransform>();
    const auto input = m_player->getComponent<CInput>();

    if (input.up || input.right || input.down || input.left || input.attack) {
        m_player->removeComponent<CPath>();
    }

    p_transform.prev_facing = p_transform.facing;

    if (m_player->hasComponent<CPath>()) {
        auto& path = m_player->getComponent<CPath>();
        const auto target = path.positions[path.cur_pos];
        if (!targetReached(p_transform.pos, target)) {
            auto desired = target - p_transform.pos;
            desired = desired.normalize();
            desired = desired*m_player_config.v;
            p_transform.velocity = desired;
            setFacing(m_player);
        } else {
            path.cur_pos++;
            if (path.cur_pos >= path.positions.size()) {
                p_transform.velocity = Vec2(0.0f, 0.0f);
                m_player->removeComponent<CPath>();
            }
        }
    } else {
        Vec2 new_velocity(0.0f, 0.0f);

        if (input.up) {
            new_velocity.y -= m_player_config.v;
            new_velocity.x = 0.0f;
        }
        if (input.down) {
            new_velocity.y += m_player_config.v;
            new_velocity.x = 0.0f;
        }
        if (input.left) {
            new_velocity.x -= m_player_config.v;
            new_velocity.y = 0.0f;
        }
        if (input.right) {
            new_velocity.x += m_player_config.v;
            new_velocity.y = 0.0f;
        }
        p_transform.velocity = new_velocity;
        setFacing(m_player);

        if (input.attack) {
            if (m_can_attack) {
                spawnSword(m_player);
            }
            m_can_attack = false;
        } else {
            m_can_attack = true;
        }
    }

    for (auto e : m_entity_manager.getEntities()){
        auto& transform = e->getComponent<CTransform>();
        transform.prev_pos = transform.pos;
        transform.pos += transform.velocity;
        // transform.angle += 0.1f; // NOTE: For crazy effects

        if (e->hasComponent<CWeapon>()) {
            const auto weapon = e->getComponent<CWeapon>();
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
            case ActionName::MOUSE_SCROLL: updateZoom(action.delta); break;
            case ActionName::MOUSE_MOVE: m_mouse_pos = action.pos; break;
            case ActionName::TOGGLE_FOLLOW: m_follow = !m_follow; break;
            case ActionName::TOGGLE_HEALTH: m_show_hp = !m_show_hp; break;
            case ActionName::TOGGLE_TEXTURE: m_draw_textures = !m_draw_textures; break;
            case ActionName::TOGGLE_COLLISION: m_draw_collision = !m_draw_collision; break;
            case ActionName::TOGGLE_GRID: m_draw_grid = !m_draw_grid; break;
            case ActionName::TOGGLE_AI_INFO: m_show_ai_info = !m_show_ai_info; break;
            case ActionName::UP: m_player->getComponent<CInput>().up = true; break;
            case ActionName::RIGHT: m_player->getComponent<CInput>().right = true; break;
            case ActionName::DOWN: m_player->getComponent<CInput>().down = true; break;
            case ActionName::LEFT: m_player->getComponent<CInput>().left = true; break;
            case ActionName::ATTACK: m_player->getComponent<CInput>().attack = true; break;
            case ActionName::L_SYSTEM: m_system_key_pressed = true; break;
            case ActionName::TOGGLE_LEVEL_EDITOR: {
                m_engine->toggleEditMode();
                m_paused = m_engine->editMode();
                break;
            }
            case ActionName::LEFT_CLICK: {
                if (!m_engine->editMode()) {
                    break;
                }

                const auto world_pos = worldPos();
                m_engine->pushSelectedPos(fitToGrid(world_pos), !m_system_key_pressed);
                for (auto e : m_entity_manager.getEntities()) {
                    if (physics::isInside(world_pos, e)) {
                        m_engine->pushSelectedEntityId(e->id(), !m_system_key_pressed);
                        if (e->hasComponent<CDraggable>()) {
                            e->getComponent<CDraggable>().dragged = true;
                        }
                        break;
                    }
                }
                break;
            }
            case ActionName::RIGHT_CLICK: {
                m_player->removeComponent<CPath>();

                const auto start = fitToGrid(m_player->getComponent<CTransform>().pos);
                const auto goal = fitToGrid(worldPos());
                // Timer t;
                // t.start();
                const auto path = path::getPath(start, goal, m_entity_manager);
                // std::cout << "Total: " << t.elapsed() <<  " us\n";
                if (path.size() > 0) {
                    m_player->addComponent<CPath>(path);
                }
                break;
            }
            default: break;
        }
    } else if (action.getType() == ActionType::END) {
        switch (action.getName()) {
            case ActionName::UP: m_player->getComponent<CInput>().up = false; break;
            case ActionName::DOWN: m_player->getComponent<CInput>().down = false; break;
            case ActionName::RIGHT: m_player->getComponent<CInput>().right = false; break;
            case ActionName::LEFT: m_player->getComponent<CInput>().left = false; break;
            case ActionName::ATTACK: m_player->getComponent<CInput>().attack = false; break;
            case ActionName::L_SYSTEM: { m_system_key_pressed = false; break; }
            case ActionName::LEFT_CLICK: {
                if (m_engine->editMode()) {
                    const auto world_pos = worldPos();
                    for (auto e : m_entity_manager.getEntities()) {
                        if (e->hasComponent<CDraggable>() && physics::isInside(world_pos, e)) {
                            e->getComponent<CDraggable>().dragged = false;
                            e->getComponent<CTransform>().pos = (fitToGrid(world_pos));
                        }
                    }
                    break;
                }
            }
            default: break;
        }
    }
}

void SceneRPG::sAI() {
    for (auto e : m_entity_manager.getEntities(Tag::ENEMY)) {
        // Patrol
        if (e->hasComponent<CPatrol>()) {
            auto& patrol = e->getComponent<CPatrol>();
            auto& transform = e->getComponent<CTransform>();

            if (patrol.positions.size() <= patrol.cur_pos) {
                continue;
            }

            auto target = patrol.positions[patrol.cur_pos];
            if (targetReached(transform.pos, target)) {
                patrol.cur_pos = patrol.cur_pos + 1 < patrol.positions.size() ? patrol.cur_pos + 1 : 0;
                target = patrol.positions[patrol.cur_pos];
            }

            auto desired = target - transform.pos;
            desired = desired.normalize();
            desired = desired*patrol.speed;
            transform.velocity = desired;
        }

        // Follow
        if (e->hasComponent<CFollowPlayer>()) {
            auto target = m_player->getComponent<CTransform>().pos;
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
                auto desired = target - transform.pos;
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
    for (const auto& entity : m_entity_manager.getEntities(Tag::TILE)) {
        if (!entity->getComponent<CBBox>().block_movement) {
            continue;
        }
        const auto overlap = physics::getOverlap(m_player, entity);
        if (overlap.x > 0 && overlap.y > 0) {
            auto prev_overlap = physics::getPrevOverlap(m_player, entity);
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

        for (const auto& sword : m_entity_manager.getEntities(Tag::SWORD)) {
            if (physics::overlapping(enemy, sword)) {
                if (!sword->hasComponent<CDamage>()) {
                    continue;
                }
                auto& hp = enemy->getComponent<CHealth>();
                hp.current -= sword->getComponent<CDamage>().damage;
                hp.percentage = static_cast<float>(hp.current)/static_cast<float>(hp.max);

                if (hp.current <= 0) {
                    enemy->destroy();
                } else {
                    enemy->addComponent<CBehavior>(true);
                    if (enemy->hasComponent<CPatrol>()) {
                        const auto pos = enemy->getComponent<CTransform>().pos;
                        const auto follow_speed = 1.0f;
                        enemy->removeComponent<CPatrol>();
                        enemy->addComponent<CFollowPlayer>(pos, follow_speed);
                    }
                }
            }
        }

        if (m_player->hasComponent<CInvincibility>()) {
            continue;
        }

        if (!enemy->hasComponent<CBehavior>()) {
            continue;
        }

        if (enemy->getComponent<CBehavior>().hostile && physics::overlapping(m_player, enemy)) {
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
            teleport(doorway->getComponent<CTransform>().pos);
            break;
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
            const auto p_transform = m_player->getComponent<CTransform>();
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
		        entity->getComponent<CAnimation>().animation.getTextureRect().setColor(Color(255, 128, 128, 128));
	        } else {
                entity->getComponent<CAnimation>().animation.getTextureRect().setColor(Color(255, 255, 255));
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
    auto view = m_engine->window().getView();
    if (m_follow) {
        // Get view from player follow camera
        const auto p_pos = m_player->getComponent<CTransform>().pos;
        view.setCenter(p_pos.x, p_pos.y);
    } else {
        // Get view for room-based camera
        const auto room = getCurrentRoom();
        view.setCenter(
            static_cast<float>((width()/2) + room.x*width()),
            static_cast<float>((height()/2) + room.y*height())
        );
    }
    if (m_zoom.level != m_zoom.prev_level) {
        m_zoom.prev_level = m_zoom.level;
        view.zoom(m_zoom.value);
    }
    m_engine->window().setView(view);
}

void SceneRPG::sRender() {
    m_engine->window().clear(Color(113, 166, 50));

    // Draw all Entity textures/animations
    renderCommon(m_player);
}

void SceneRPG::sDragAndDrop() {
    for (auto e : m_entity_manager.getEntities()) {
        if (e->hasComponent<CDraggable>() && e->getComponent<CDraggable>().dragged) {
            e->getComponent<CTransform>().pos = worldPos();
        }
    }
}

void SceneRPG::sInteract() {
    for (auto e : m_entity_manager.getEntities()) {
        if (!e->hasComponent<CInteractable>()) {
            continue;
        }
        auto& highlight = e->getComponent<CInteractable>().highlight;
        if (physics::isInside(m_mouse_pos, e)) {
            highlight = true;
        } else {
            highlight = false;
        }
    }
}

void SceneRPG::onEnd() {
    // Reset view
    m_engine->window().setDefaultView();

    // Go back to menu
    m_engine->changeScene(SceneType::MENU, std::make_shared<SceneMenu>(m_engine), true);
}
