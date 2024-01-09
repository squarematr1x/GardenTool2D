#include <string>
#include <fstream>

#include "Scene.h"
#include "GameEngine.h"
#include "Physics.h"

ScenePlay::ScenePlay(GameEngine* engine, const std::string& level_path)
    : Scene(engine), m_level_path(level_path) {
    init(level_path);
}

void ScenePlay::init(const std::string& level_path) {
    registerAction(sf::Keyboard::P, ActionName::PAUSE);
    registerAction(sf::Keyboard::Escape, ActionName::QUIT);
    registerAction(sf::Keyboard::T, ActionName::TOGGLE_TEXTURE);
    registerAction(sf::Keyboard::C, ActionName::TOGGLE_COLLISION);
    registerAction(sf::Keyboard::G, ActionName::TOGGLE_GRID);

    registerAction(sf::Keyboard::Space, ActionName::UP);
    registerAction(sf::Keyboard::Up, ActionName::UP);
    registerAction(sf::Keyboard::Right, ActionName::RIGHT);
    registerAction(sf::Keyboard::Down, ActionName::DOWN);
    registerAction(sf::Keyboard::Left, ActionName::LEFT);
    registerAction(sf::Keyboard::Z, ActionName::SHOOT);

    m_grid_text.setCharacterSize(12);
    m_grid_text.setFont(m_engine->assets().getFont("Arial"));
    m_pause_text.setCharacterSize(20);
    m_pause_text.setFont(m_engine->assets().getFont("Arial"));

    loadLevel(level_path);
}

Vec2 ScenePlay::gridToMidPixel(float grid_x, float grid_y, std::shared_ptr<Entity> entity) {
    if (entity->hasComponent<CAnimation>()) {
        const auto& animation_size = entity->getComponent<CAnimation>().animation.getSize();
        const float x = grid_x * m_grid_size.x + (animation_size.x / 2.0f);
        const float y = height() - (grid_y * m_grid_size.y + (animation_size.y / 2.0f));

        return Vec2(x, y);
    }
    return Vec2(0, 0);
}

void ScenePlay::loadLevel(const std::string& path) {
    // reset the entity manager every time we load a level
    m_entity_manager = EntityManager();

    std::ifstream file(path);
    std::string str;

    while (file.good()) {
        file >> str;

        if (str == "Tile" || str == "Dec") {
            std::string animation;
            float x, y;
            file >> animation >> x >> y;

            auto tile = str == "Tile" ? m_entity_manager.addEntity(Tag::TILE) : m_entity_manager.addEntity(Tag::DEC);
            tile->addComponent<CAnimation>(m_engine->assets().getAnimation(animation), true);
            tile->addComponent<CTransform>(gridToMidPixel(x, y, tile));

            if (tile->getComponent<CAnimation>().animation.getName() == "Brick") {
                const auto& animation_size = tile->getComponent<CAnimation>().animation.getSize();
                tile->addComponent<CBBox>(animation_size);
            }
        } else if (str == "Player") {
            float x, y, bbox_w, bbox_h, v, jump_v, max_v, gravity;
            std::string weapon_animation;
            file >> x >> y >> bbox_w >> bbox_h >> v >> max_v >> jump_v >> gravity >> weapon_animation;
            m_player_config = {
                x, y, bbox_w, bbox_h, v, max_v, jump_v, gravity, weapon_animation
            };
        } else {
            std::cerr << "Unknown level object: " << str << '\n';
        }
    }

    spawnPlayer();
}

void ScenePlay::spawnPlayer() {
    m_player = m_entity_manager.addEntity(Tag::PLAYER);
    m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("Stand"), true);
    m_player->addComponent<CTransform>(gridToMidPixel(m_player_config.x, m_player_config.y, m_player));
    m_player->addComponent<CBBox>(Vec2(m_player_config.bbox_x, m_player_config.bbox_y));
    m_player->addComponent<CGravity>(m_player_config.gravity);
}

void ScenePlay::spawnBullet() {
    auto transform = m_player->getComponent<CTransform>();

    auto bullet = m_entity_manager.addEntity(Tag::BULLET);
    bullet->addComponent<CAnimation>(m_engine->assets().getAnimation("Fire"), true);

    constexpr float bullet_v = 3.0f; // TODO: Add to config file?
    if (transform.scale.x < 0) {
        bullet->addComponent<CTransform>(transform.pos, Vec2(-bullet_v, 0.0f));
    } else {
        bullet->addComponent<CTransform>(transform.pos, Vec2(bullet_v, 0.0f));
    }
    // bullet->addComponent<CBBox>(Vec2(m_player_config.bbox_x, m_player_config.bbox_y));
}

void ScenePlay::update() {
    if (!m_paused) {
        m_entity_manager.update();

        sMovement();
        sLifespan();
        sCollision();
        sAnimation();
    }
    sRender();
}

void ScenePlay::sMovement() {
    Vec2 player_v = m_player->getComponent<CTransform>().velocity;

    auto& input = m_player->getComponent<CInput>();

    player_v.x = 0.0f;
    if (input.up) {
        m_player->getComponent<CState>().state = State::JUMP;
        player_v.y = -m_player_config.jump_v;
    }
    if (input.left) {
        player_v.x = -m_player_config.v;
        m_player->getComponent<CTransform>().scale = Vec2(-1, 1);
    }
    if (input.right) {
        player_v.x = m_player_config.v;
         m_player->getComponent<CTransform>().scale = Vec2(1, 1);
    }
    if (input.can_shoot && input.shoot) {
        input.can_shoot = false;
        input.shoot = false;
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
    }

    // TODO: Implement player movement/jumping based on its CInput component
    // TODO: Setting an entity's scale.x to -1/1 will make it face to the left/right
}

void ScenePlay::sLifespan() {
    for (auto entity : m_entity_manager.getEntities()) {
        if (entity->hasComponent<CLifespan>()) {
            if (entity->getComponent<CLifespan>().remaining <= 0) {
                entity->destroy();
            }
        }
    }
}

void ScenePlay::sCollision() { // TODO: Use EState to detect direction of the player etc. and utilize this in movement and spawn bullet also
    for (auto entity : m_entity_manager.getEntities(Tag::TILE)) {
        Vec2 overlap = physics::getOverlap(m_player, entity);
        if (overlap.x > 0 && overlap.y > 0) {
            Vec2 prev_overlap = physics::getPrevOverlap(m_player, entity);
            auto& transform = m_player->getComponent<CTransform>();
            if (prev_overlap.y > 0) {
                if (transform.velocity.x > 0) { transform.pos.x -= overlap.x; }
                else if (transform.velocity.x < 0) { transform.pos.x += overlap.x; }
                transform.velocity.x = 0.0f;
            }
            if (prev_overlap.x > 0) {
                if (transform.velocity.y > 0) { transform.pos.y -= overlap.y; }
                else if (transform.velocity.y < 0) { transform.pos.y += overlap.y; }
                transform.velocity.y = 0.0f;
            }
        }
    }

    if (m_player->getComponent<CTransform>().pos.y > height()) {
        m_player->addComponent<CTransform>().pos = gridToMidPixel(m_player_config.x, m_player_config.y, m_player);
    }
    if (m_player->getComponent<CTransform>().pos.x < m_player->getComponent<CBBox>().half_size.x) {
        m_player->getComponent<CTransform>().pos.x = m_player->getComponent<CBBox>().half_size.x;
    }

    // TODO: Implement bullet/tile collision (Destroy the tile if it has 'Brick' as animation)
    // TODO: Implement player/tile collision and resolutions, update the CState component of the player to
    // store whether it's currently on the ground or in the air. This will be used by the animation system.
}

void ScenePlay::sDoAction(const Action& action) {
    switch (action.getType()) {
        case ActionType::START: {
            switch (action.getName()) {
                case ActionName::TOGGLE_TEXTURE: m_draw_textures = !m_draw_textures; break;
                case ActionName::TOGGLE_COLLISION: m_draw_collision = !m_draw_collision; break;
                case ActionName::TOGGLE_GRID: m_draw_grid = !m_draw_grid; break;
                case ActionName::PAUSE: setPaused(!m_paused); break;
                case ActionName::UP: m_player->getComponent<CInput>().up = true; break;
                case ActionName::RIGHT: m_player->getComponent<CInput>().right = true; break;
                case ActionName::DOWN: m_player->getComponent<CInput>().down = true; break;
                case ActionName::LEFT: m_player->getComponent<CInput>().left = true; break;
                case ActionName::SHOOT: m_player->getComponent<CInput>().shoot = true; break;
                case ActionName::QUIT: onEnd(); break;
                default: break;
            }
            break;
        }
        case ActionType::END: {
            switch (action.getName()) {
                case ActionName::UP: m_player->getComponent<CInput>().up = false; break;
                case ActionName::RIGHT: m_player->getComponent<CInput>().right = false; break;
                case ActionName::DOWN: m_player->getComponent<CInput>().down = false; break;
                case ActionName::LEFT: m_player->getComponent<CInput>().left = false; break;
                case ActionName::SHOOT:
                    m_player->getComponent<CInput>().can_shoot = true;
                    m_player->getComponent<CInput>().shoot = false; 
                    break;
                default: break;
            }
            break;
        }
        default: break;
    }
}

void ScenePlay::sAnimation() {
    // TODO: Complete the animation class code first
    // TODO: set the animation of the player based on its CState component
    // TODO: for each entity with an animation, call entity->getComponent<CAnimation>().animation.update();
    //       if the animation is not repeated, and it has ended, destroy the entity

    // if (m_player->getComponent<CState>().state == "JUMP") { // TODO: This cause segfault (source: lldb)
    //     m_player->addComponent<CAnimation>(m_game_engine->assets().getAnimation("Jump"));
    // }

    // if (m_player->getComponent<CState>().state == "RUN") {
    //     m_player->addComponent<CAnimation>(m_game_engine->assets().getAnimation("Run"));
    // }
}

void ScenePlay::sRender() {
    if (m_paused) { m_engine->window().clear(sf::Color(50, 50, 150)); }
    else { m_engine->window().clear(sf::Color(70, 80, 255)); }

    // set the viewport of the window to be centered on the player if it's far enough right
    auto& p_pos = m_player->getComponent<CTransform>().pos;
    float window_center_x = std::max(m_engine->window().getSize().x / 2.0f, p_pos.x);
    sf::View view = m_engine->window().getView();
    view.setCenter(window_center_x, m_engine->window().getSize().y - view.getCenter().y);
    m_engine->window().setView(view);

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
            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangle shape
    if (m_draw_collision) {
        for (auto e : m_entity_manager.getEntities()) {
            if (e->hasComponent<CBBox>()) {
                auto& box = e->getComponent<CBBox>();
                auto& transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x-1, box.size.y-1));
                rect.setOrigin(sf::Vector2f(box.half_size.x, box.half_size.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(255, 255, 255, 0));
                rect.setOutlineColor(sf::Color(255, 255, 255, 255));
                rect.setOutlineThickness(1);
                m_engine->window().draw(rect);
            }
        }
    }

    if (m_draw_grid) {
        float left_x = m_engine->window().getView().getCenter().x - width() / 2;
        float right_x = left_x + width() + m_grid_size.x;
        float next_grid_x = left_x - (static_cast<int>(left_x) % static_cast<int>(m_grid_size.x));

        for (float x = next_grid_x; x < right_x; x += m_grid_size.x) {
            drawLine(Vec2(x, 0.0f), Vec2(x, height()));
        }

        for (float y = 0; y < height(); y += m_grid_size.y) {
            drawLine(Vec2(left_x, height() - y), Vec2(right_x, height() - y));

            for (float x = next_grid_x; x < right_x; x += m_grid_size.x) {
                std::string x_cell = std::to_string(static_cast<int>(x) / static_cast<int>(m_grid_size.x));
                std::string y_cell = std::to_string(static_cast<int>(y) / static_cast<int>(m_grid_size.y));
                m_grid_text.setString("(" + x_cell + "," + y_cell + ")");
                m_grid_text.setPosition(x + 3, height() - y - m_grid_size.y + 2);
                m_engine->window().draw(m_grid_text);
            }
        }
    }

    if (m_paused) {
        const auto text_rect = m_pause_text.getLocalBounds();
        m_pause_text.setString("PAUSE");
        m_pause_text.setOrigin(text_rect.left + text_rect.width/2.0f, text_rect.top + text_rect.height/2.0f);
        m_pause_text.setPosition(width()/2.0f, height()/2.0f);
        m_engine->window().draw(m_pause_text);
    }
}

void ScenePlay::onEnd() {
    // Reset view
    sf::View view = m_engine->window().getView();
    view.setCenter(m_engine->window().getSize().x / 2.0f, m_engine->window().getSize().y - view.getCenter().y);
    m_engine->window().setView(view);

    m_engine->changeScene(SceneType::MENU, std::make_shared<SceneMenu>(m_engine), true);
}
