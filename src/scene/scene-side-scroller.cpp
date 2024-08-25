#include <string>
#include <fstream>
#include <sstream>

#include "scene.hpp"
#include "../engine.hpp"
#include "../physics.hpp"

SceneSideScroller::SceneSideScroller(GameEngine* engine, const std::string& level_path)
    : Scene(engine, level_path) {
    init(level_path);
}

void SceneSideScroller::init(const std::string& level_path) {
    registerAction(sf::Keyboard::P, ActionName::PAUSE);
    registerAction(sf::Keyboard::Escape, ActionName::QUIT);
    registerAction(sf::Keyboard::T, ActionName::TOGGLE_TEXTURE);
    registerAction(sf::Keyboard::X, ActionName::TOGGLE_COLLISION);
    registerAction(sf::Keyboard::G, ActionName::TOGGLE_GRID);
    registerAction(sf::Keyboard::Tab, ActionName::TOGGLE_LEVEL_EDITOR);

    registerAction(sf::Keyboard::Space, ActionName::UP);
    registerAction(sf::Keyboard::Up, ActionName::UP);
    registerAction(sf::Keyboard::Right, ActionName::RIGHT);
    registerAction(sf::Keyboard::Down, ActionName::DOWN);
    registerAction(sf::Keyboard::Left, ActionName::LEFT);
    registerAction(sf::Keyboard::Z, ActionName::SHOOT);

    registerAction(sf::Mouse::Button::Left, ActionName::LEFT_CLICK);
    registerAction(sf::Mouse::Button::Middle, ActionName::MIDDLE_CLICK);
    registerAction(sf::Mouse::Button::Right, ActionName::RIGHT_CLICK);
    registerAction(sf::Mouse::Wheel::VerticalWheel, ActionName::MOUSE_SCROLL);

    m_grid_text.setCharacterSize(12);
    m_grid_text.setFont(m_engine->assets().getFont("Arial"));
    m_pause_text.setCharacterSize(20);
    m_pause_text.setFont(m_engine->assets().getFont("Arial"));

    loadLevel(level_path);

    // m_engine->playMusic("Level1Music");
}

Vec2 SceneSideScroller::gridToMidPixel(float grid_x, float grid_y, std::shared_ptr<Entity> entity) {
    if (entity->hasComponent<CAnimation>()) {
        const auto animation_size = entity->getComponent<CAnimation>().animation.getSize();
        const float x = grid_x * m_grid_size.x + (animation_size.x / 2.0f);
        const float y = height() - (grid_y * m_grid_size.y + (animation_size.y / 2.0f));

        return Vec2(x, y);
    }
    if (entity->hasComponent<CBBox>()) {
        const auto half_size = entity->getComponent<CBBox>().half_size;
        const float x = grid_x * m_grid_size.x + half_size.x;
        const float y = height() - (grid_y * m_grid_size.y + half_size.y);

        return Vec2(x, y);
    }
    return Vec2(0, 0);
}

Vec2 SceneSideScroller::fitToGrid(const Vec2& pos) const {
    return Vec2(
        floorf(pos.x/m_grid_size.x)*m_grid_size.x + m_grid_size.x/2,
        floorf(pos.y/m_grid_size.y)*m_grid_size.y + m_grid_size.y/2
    );
}

// NOTE: Doesn't take zooming into account
Vec2 SceneSideScroller::mouseToWorldPos(const Vec2& mouse_pos) const {
    auto view = m_engine->window().getView();
    float world_x = view.getCenter().x - (m_engine->window().getSize().x/2);
    float world_y = view.getCenter().y - (m_engine->window().getSize().y/2);

    return Vec2(mouse_pos.x + world_x, mouse_pos.y + world_y);
}

void SceneSideScroller::loadLevel(const std::string& path) {
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

                auto tile = block_movement ? m_entity_manager.addEntity(Tag::TILE) : m_entity_manager.addEntity(Tag::DEC);
                tile->addComponent<CAnimation>(m_engine->assets().getAnimation(animation), true);
                tile->addComponent<CTransform>(gridToMidPixel(x, y, tile));
                tile->addComponent<CDraggable>(); // TODO: Add draggable to other entities later
                if (block_movement) {
                    const auto& animation_size = tile->getComponent<CAnimation>().animation.getSize();
                    if (animation == "Brick") {
                        tile->addComponent<CBBox>(animation_size, block_movement, block_vision, true);
                    } else {
                        tile->addComponent<CBBox>(animation_size, block_movement, block_vision);
                    }
                }
            } else if (asset_type == "Player") {
                float x, y, bbox_w, bbox_h, v, jump_v, max_v, gravity;
                std::string weapon_animation;
                text_stream >> x >> y >> bbox_w >> bbox_h >> v >> max_v >> jump_v >> gravity >> weapon_animation;
                m_player_config = {
                    x, y, bbox_w, bbox_h, v, max_v, jump_v, gravity, weapon_animation
                };
            } else if (asset_type == "Elevator") {
                std::string animation, mode;
                float x, y;
                bool block_movement, block_vision;
                text_stream >> animation >> x >> y >> block_movement >> block_vision;
                auto tile = m_entity_manager.addEntity(Tag::ELEVATOR);
                tile->addComponent<CAnimation>(m_engine->assets().getAnimation(animation), true);
                tile->addComponent<CTransform>(gridToMidPixel(x, y, tile));
                if (block_movement) {
                    const auto& animation_size = tile->getComponent<CAnimation>().animation.getSize();
                    tile->addComponent<CBBox>(animation_size, block_movement, block_vision);
                }

                text_stream >> mode;
                if (mode == "Patrol") {
                    float speed;
                    int n_positions;
                    std::vector<Vec2> positions;
                    text_stream >> speed >> n_positions;
                    for (int i = 0; i < n_positions; i++) {
                        text_stream >> x >> y;
                        positions.push_back(gridToMidPixel(x, y, tile));
                    }
                    tile->addComponent<CPatrol>(positions, speed);
                } else if (mode == "Follow") {
                    float speed, y, x;
                    text_stream >> speed >> x >> y;
                    tile->addComponent<CFollowPlayer>(gridToMidPixel(x, y, tile), speed);
                }
            } else if (asset_type == "NPC") { 
                std::string animation, mode;
                float x, y;
                bool block_movement, block_vision;
                int hp, damage;
                text_stream >> animation >> x >> y >> block_movement >> block_vision >> hp >> damage;
                auto enemy = m_entity_manager.addEntity(Tag::ENEMY);
                enemy->addComponent<CAnimation>(m_engine->assets().getAnimation(animation), true);
                enemy->addComponent<CTransform>(gridToMidPixel(x, y, enemy), true);
                enemy->addComponent<CHealth>(hp);
                enemy->addComponent<CDamage>(damage);
                const auto& animation_size = enemy->getComponent<CAnimation>().animation.getSize();
                enemy->addComponent<CBBox>(animation_size, block_movement, block_vision);

                text_stream >> mode;
                if (mode == "Patrol") {
                    float speed;
                    int n_positions;
                    std::vector<Vec2> positions;
                    text_stream >> speed >> n_positions;
                    for (int i = 0; i < n_positions; i++) {
                        text_stream >> x >> y;
                        positions.push_back(gridToMidPixel(x, y, enemy));
                    }
                    enemy->addComponent<CPatrol>(positions, speed);
                } else if (mode == "Follow") {
                    float speed, y, x;
                    text_stream >> speed >> x >> y;
                    enemy->addComponent<CFollowPlayer>(gridToMidPixel(x, y, enemy), speed);
                }
            } else if (asset_type == "Checkpoint") {
                float x, y, bbox_w, bbox_h;
                text_stream >> x >> y >> bbox_w >> bbox_h;
                auto checkpoint = m_entity_manager.addEntity(Tag::CHECKPOINT);
                checkpoint->addComponent<CBBox>(Vec2(bbox_w, bbox_h));
                checkpoint->addComponent<CTransform>(gridToMidPixel(x, y, checkpoint));
            } else if (asset_type == "Background") {
                std::string layer;
                text_stream >> layer;
                m_background_layers.push_back(m_engine->assets().getLayer(layer));
            } else {
                std::cerr << "Unknown level object: " << asset_type << '\n';
                // TODO: handle this error
            }
        }
    }
    file.close();

    spawnPlayer();
}

void SceneSideScroller::spawnPlayer() {
    m_player = m_entity_manager.addEntity(Tag::PLAYER);
    m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("Tooth"), true);
    m_player->addComponent<CTransform>(gridToMidPixel(m_player_config.x, m_player_config.y, m_player), true);
    m_player->addComponent<CBBox>(Vec2(m_player_config.bbox_x, m_player_config.bbox_y));
    m_player->addComponent<CGravity>(m_player_config.gravity);
}

void SceneSideScroller::spawnBullet() {
    auto transform = m_player->getComponent<CTransform>();

    auto bullet = m_entity_manager.addEntity(Tag::BULLET);
    bullet->addComponent<CAnimation>(m_engine->assets().getAnimation("Fire"), true);

    constexpr float bullet_v = 4.0f; // TODO: Add to config file?
    if (transform.scale.x < 0) {
        bullet->addComponent<CTransform>(transform.pos, Vec2(-bullet_v, 0.0f));
    } else {
        bullet->addComponent<CTransform>(transform.pos, Vec2(bullet_v, 0.0f));
    }
    bullet->addComponent<CBBox>(Vec2(32.0f, 32.0f));
    bullet->addComponent<CLifespan>(60);
}

void SceneSideScroller::spawnExplosion(const Vec2& pos) {
    auto explosion = m_entity_manager.addEntity(Tag::EXPLOSION);
    explosion->addComponent<CAnimation>(m_engine->assets().getAnimation("Explosion"));
    explosion->addComponent<CTransform>(pos, Vec2(0, 0));
}

void SceneSideScroller::spawnItem(const Vec2& pos, const std::string& animation_name, Tag tag) {
    auto item = m_entity_manager.addEntity(tag);
    item->addComponent<CAnimation>(m_engine->assets().getAnimation(animation_name), true);
    item->addComponent<CTransform>(pos, Vec2(0, 0));
}

void SceneSideScroller::update() {
    if (!m_paused) {
        m_entity_manager.update();

        sAI();
        sMovement();
        sLifespan();
        sCollision();
        sAnimation();
        sDragAndDrop();
        sCamera();
    }
    // sf::View mini_map = m_engine->window().getView();
    // mini_map.setViewport(sf::FloatRect(0.75f, 0.0f, 0.25f, 0.25f));
    // m_engine->window().setView(mini_map);
    sRender();

    m_current_frame++;
}

void SceneSideScroller::sAI() {
    for (auto e : m_entity_manager.getEntities()) {
        if (e->tag() != Tag::ENEMY && e->tag() != Tag::ELEVATOR) {
            continue;
        }
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
    }
}

void SceneSideScroller::sMovement() {
    Vec2 player_v = m_player->getComponent<CTransform>().velocity;
    auto& player_state = m_player->getComponent<CState>().state;
    auto& input = m_player->getComponent<CInput>();

    player_v.x = 0.0f;

    if (!input.up && player_state != State::JUMP) {
        m_can_jump = true;
    } else if (input.up && m_can_jump && player_state != State::JUMP) {
        m_can_jump = false;
        player_v.y = -m_player_config.jump_v;
        m_engine->playSound("SoundJump");
    }

    if (player_v.y != 0.0f) {
        player_state = State::JUMP;
    }
    
    if (input.left) {
        player_v.x = -m_player_config.v;
        if (player_v.y == 0 && player_state != State::JUMP) {
            player_state = State::RUN;
        }
    }
    if (input.right) {
        player_v.x = m_player_config.v;
        if (player_v.y == 0 && player_state != State::JUMP) {
            player_state = State::RUN;
        }
    }

    if (!input.attack) {
        m_can_shoot = true;
    } else if (input.attack && m_can_shoot) {
        m_can_shoot = false;
        spawnBullet();
    }

    if (player_v.x > m_player_config.max_v) {
        player_v.x = m_player_config.max_v;
    } else if (player_v.x < -m_player_config.max_v) {
        player_v.x = -m_player_config.max_v;
    }
    if (player_v.y > m_player_config.max_v) {
        player_v.y = m_player_config.max_v;
    } else if (player_v.x < -m_player_config.max_v) {
        player_v.y = -m_player_config.max_v;
    }

    m_player->getComponent<CTransform>().velocity = player_v;
    
    for (auto e : m_entity_manager.getEntities()){
        if (e->hasComponent<CGravity>()) {
            e->getComponent<CTransform>().velocity.y += e->getComponent<CGravity>().gravity;
        }
        auto& transform = e->getComponent<CTransform>();
        transform.prev_pos = transform.pos;
        transform.pos += transform.velocity;

        if (transform.velocity.x < 0) {
            transform.scale.x = -fabsf(transform.scale.x);
        } else if (transform.velocity.x > 0)  {
           transform.scale.x = fabsf(transform.scale.x); 
        }
    }
}

void SceneSideScroller::sLifespan() {
    for (auto entity : m_entity_manager.getEntities()) {
        if (entity->hasComponent<CLifespan>()) {
            if (entity->getComponent<CLifespan>().remaining <= 0) {
                entity->destroy();
            }
            // Destroy bullets, if they don't collide with anything
            entity->getComponent<CLifespan>().remaining--;
        }
    }
}

void SceneSideScroller::sCollision() {
    auto& p_transfrom = m_player->getComponent<CTransform>();
    auto& p_state = m_player->getComponent<CState>();
    auto& p_bbox = m_player->getComponent<CBBox>();

    for (auto entity : m_entity_manager.getEntities()) {
        if (entity->tag() != Tag::TILE &&
            entity->tag() != Tag::ELEVATOR &&
            entity->tag() != Tag::CHECKPOINT) {
            continue;
        }

        // Checkpoint
        if (entity->tag() == Tag::CHECKPOINT) {
            if (physics::overlapping(m_player, entity)) {
                auto checkpointPos = entity->getComponent<CTransform>().pos;
                m_player_config.x = (checkpointPos.x - (m_grid_size.x / 2.0f)) / m_grid_size.x;
                m_player_config.y = ((height() - checkpointPos.y - (m_grid_size.y / 2.0f)) / m_grid_size.y);
            }
        }

        if (!entity->getComponent<CBBox>().block_movement) {
            continue;
        }
    
        // Player collision
        Vec2 overlap = physics::getOverlap(m_player, entity);
        if (overlap.x > 0 && overlap.y > 0) {
            Vec2 prev_overlap = physics::getPrevOverlap(m_player, entity);
            if (prev_overlap.y > 0) {
                if (p_transfrom.velocity.x > 0) {
                    p_transfrom.pos.x -= overlap.x;
                }
                else if (p_transfrom.velocity.x < 0) {
                    p_transfrom.pos.x += overlap.x;
                }
                p_transfrom.velocity.x = 0.0f;
            }
            if (prev_overlap.x > 0) {
                if (p_transfrom.velocity.y > 0) {
                    p_transfrom.pos.y -= overlap.y;
                    if (p_transfrom.velocity.x != 0.0f) {
                        p_state.state = State::RUN;
                    }
                    else {
                        p_state.state = State::STAND;
                    }
                    if (entity->tag() == Tag::ELEVATOR) {
                        p_transfrom.pos.x += entity->getComponent<CTransform>().velocity.x;
                    }
                } else if (p_transfrom.velocity.y < 0) {
                    p_transfrom.pos.y += overlap.y;
                    if (entity->getComponent<CBBox>().breakable) {
                        spawnExplosion(entity->getComponent<CTransform>().pos);
                        entity->destroy();
                    } else if (entity->getComponent<CAnimation>().animation.getName() == "Question1") {
                        auto e_pos = entity->getComponent<CTransform>().pos;
                        spawnItem(Vec2(e_pos.x, e_pos.y - 64.0f), "Heart", Tag::HEART);
                        entity->addComponent<CAnimation>(m_engine->assets().getAnimation("Question2"), true);
                    }
                }
                p_transfrom.velocity.y = 0.0f;
            }
        }

        // Bullet collision
        for (auto bullet : m_entity_manager.getEntities(Tag::BULLET)) {
            if (physics::overlapping(bullet, entity)) {
                if (entity->getComponent<CBBox>().breakable) {
                    // Destroy tile and bullet
                    spawnExplosion(entity->getComponent<CTransform>().pos);
                    entity->destroy();
                } else {
                    spawnExplosion(bullet->getComponent<CTransform>().pos);
                }
                bullet->destroy();
            }
        }
    }

    if (p_transfrom.pos.y > height()) {
        p_transfrom.pos = gridToMidPixel(m_player_config.x, m_player_config.y, m_player);
    }
    if (p_transfrom.pos.x < p_bbox.half_size.x) {
        p_transfrom.pos.x = p_bbox.half_size.x;
        p_transfrom.velocity.x = 0.0f;
    }
}

void SceneSideScroller::sDoAction(const Action& action) {
    if (action.getType() == ActionType::START) {
        switch (action.getName()) {
            case ActionName::TOGGLE_TEXTURE: m_draw_textures = !m_draw_textures; break;
            case ActionName::TOGGLE_COLLISION: m_draw_collision = !m_draw_collision; break;
            case ActionName::TOGGLE_GRID: m_draw_grid = !m_draw_grid; break;
            case ActionName::PAUSE: setPaused(!m_paused); break;
            case ActionName::QUIT: onEnd(); break;
            case ActionName::UP: m_player->getComponent<CInput>().up = true; break;
            case ActionName::RIGHT: m_player->getComponent<CInput>().right = true; break;
            case ActionName::DOWN: m_player->getComponent<CInput>().down = true; break;
            case ActionName::LEFT: m_player->getComponent<CInput>().left = true; break;
            case ActionName::SHOOT: m_player->getComponent<CInput>().attack = true; break;
            case ActionName::MOUSE_MOVE: m_mouse_pos = action.pos; m_mouse_shape.setPosition(m_mouse_pos.x, m_mouse_pos.y); break;
            case ActionName::MOUSE_SCROLL: updateZoom(action.delta); break;
            case ActionName::MIDDLE_CLICK: break;
            case ActionName::RIGHT_CLICK: {
                if (!m_engine->editMode()) {
                    break;
                }
                Vec2 world_pos = mouseToWorldPos(action.pos);
                auto tile = m_entity_manager.addEntity(Tag::TILE);
                tile->addComponent<CAnimation>(m_engine->assets().getAnimation("Brick"), true);
                tile->addComponent<CTransform>(fitToGrid(world_pos));
                tile->addComponent<CDraggable>();
                tile->addComponent<CBBox>(tile->getComponent<CAnimation>().animation.getSize(), true, true, true);
                // TODO: Get grid pos of clicked pos and open GUI
                break;
            }
            case ActionName::LEFT_CLICK: {
                if (m_engine->editMode()) {
                    Vec2 world_pos = mouseToWorldPos(action.pos);
                    for (auto e : m_entity_manager.getEntities()) {
                        if (physics::isInside(world_pos, e)) {
                            m_engine->setSelectedEntityId(e->id()); // Popup UI for the selected entity
                        }
                    }
                }
                break;
            }
            case ActionName::TOGGLE_LEVEL_EDITOR: {
                m_engine->toggleEditMode();
                m_paused = m_engine->editMode();
                break;
            }
            default: break;
        } 
    } else if (action.getType() == ActionType::END) {
        switch (action.getName()) {
            case ActionName::RIGHT: m_player->getComponent<CInput>().right = false; break;
            case ActionName::DOWN: m_player->getComponent<CInput>().down = false; break;
            case ActionName::LEFT: m_player->getComponent<CInput>().left = false; break;
            case ActionName::UP: m_player->getComponent<CInput>().up = false; break;
            case ActionName::SHOOT: m_player->getComponent<CInput>().attack = false; break;
            case ActionName::LEFT_CLICK: {
                if (m_engine->editMode()) {
                    Vec2 world_pos = mouseToWorldPos(action.pos);
                    for (auto e : m_entity_manager.getEntities()) {
                        if (physics::isInside(world_pos, e)) {
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

void SceneSideScroller::sAnimation() {
    for (auto entity : m_entity_manager.getEntities()) {
        if (!entity->hasComponent<CAnimation>()) {
            continue;
        }

        auto& e_animation = entity->getComponent<CAnimation>();
        e_animation.animation.update();

        if (!e_animation.repeat && e_animation.animation.hasEnded()) {
            entity->destroy();
        }

        if (entity->tag() == Tag::PLAYER) {
            auto& p_state = m_player->getComponent<CState>();
            if (p_state.state != p_state.prev_state) {
                switch (p_state.state) {
                    case State::STAND:
                        m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("Tooth"), true); break;
                    case State::RUN:
                        m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("ToothRun"), true); break;
                    case State::JUMP:
                        m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("ToothJump"), true); break;
                    default: break;
                }
            }
            p_state.prev_state = p_state.state;
        }
    }
}

void SceneSideScroller::sCamera() {
    auto& p_pos = m_player->getComponent<CTransform>().pos;
    float window_center_x = std::max(m_engine->window().getSize().x / 2.0f, p_pos.x);
    sf::View view = m_engine->window().getView();

    view.setCenter(window_center_x, m_engine->window().getSize().y - view.getCenter().y);
    
    if (m_zoom.level != m_zoom.prev_level) {
        m_zoom.prev_level = m_zoom.level;
        view.zoom(m_zoom.value);
    }

    m_engine->window().setView(view);
}

void SceneSideScroller::sRender() {
    m_engine->window().clear(sf::Color(236, 115, 22));

    // Draw backgrounds
    sf::View view = m_engine->window().getView();
    float parallax_velocity = 0.01f;
    const auto p_transform = m_player->getComponent<CTransform>();
    for (auto& layer : m_background_layers) {
        layer.update(p_transform.velocity.x, parallax_velocity);
        parallax_velocity += 0.05f;
        if (m_current_frame == 0) {
            layer.scale(m_engine->window().getSize().x, m_engine->window().getSize().y);
        }
        m_engine->window().setView(layer.getView());
        m_engine->window().draw(layer.getSprite());
    }
    m_engine->window().setView(view);

    // Draw all Entity textures/animations
    if (m_draw_textures) {
        sf::VertexArray vertices(sf::Triangles);
        for (auto e : m_entity_manager.getEntities()) {
            if (!e->hasComponent<CAnimation>()) {
                continue;
            }
            auto& transform = e->getComponent<CTransform>();
            auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
            if (transform.transformable) {
                sprite.setRotation(transform.angle);
                sprite.setPosition(transform.pos.x, transform.pos.y);
                sprite.setScale(transform.scale.x, transform.scale.y);
                m_engine->window().draw(sprite);
            } else {
                addVertexData(transform.pos, sprite.getTextureRect(), vertices);
            }
        }
        // Draw vertex array
        sf::RenderStates states(&m_engine->assets().getTexture("Tilemap"));
        m_engine->window().draw(vertices, states);
    }

    if (m_draw_collision || m_engine->editMode()) {
        renderBBoxes();
    }

    if (m_draw_grid || m_engine->editMode()) {
        renderGrid(m_grid_size, m_grid_text);
    }
}

void SceneSideScroller::sDragAndDrop() {
    for (auto e : m_entity_manager.getEntities()) {
        if (e->hasComponent<CDraggable>() && e->getComponent<CDraggable>().dragged) {
            Vec2 world_pos = mouseToWorldPos(m_mouse_pos);
            e->getComponent<CTransform>().pos = world_pos;
        }
    }
}

void SceneSideScroller::onEnd() {
    // Stop music
    m_engine->stopMusic("Level1Music");

    // Reset view
    m_engine->window().setView(m_engine->window().getDefaultView());

    // Go back to menu
    m_engine->changeScene(SceneType::MENU, std::make_shared<SceneMenu>(m_engine), true);
}
