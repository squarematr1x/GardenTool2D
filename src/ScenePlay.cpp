#include <string>
#include <fstream>

#include "Scene.h"
#include "GameEngine.h"

ScenePlay::ScenePlay(GameEngine* engine, const std::string& level_path)
    : Scene(engine), m_level_path(level_path) {
        init(level_path);
    }

void ScenePlay::init(const std::string& level_path) {
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");

    // TODO: register all other gameplay Actions
    registerAction(sf::Keyboard::Space, "JUMP");
    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::Right, "RIGHT");
    registerAction(sf::Keyboard::Down, "DOWN");
    registerAction(sf::Keyboard::Left, "LEFT");

    m_grid_text.setCharacterSize(12);
    
    m_grid_text.setFont(m_engine->assets().getFont("Arial"));

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
    (void)path;
    // reset the entity manager every time we load a level
    m_entity_manager = EntityManager();

    // TODO: read in the level file and add the appropriate entities
    // use the PlayerConfig struct m_playerConfig to store player properties
    // this struct is defined at the top of ScenePlay.h

    // NOTE: all of thre code below is sample code which shows you how to set up and use entitties with the new syntax

    spawnPlayer();

    std::ifstream file(path);
    std::string str;

    while (file.good()) {
        file >> str;

        if (str == "Tile" || str == "Dec") {
            std::string animation;
            float x, y;
            file >> animation >> x >> y;

            auto tile = m_entity_manager.addEntity("tile");
            tile->addComponent<CAnimation>(m_engine->assets().getAnimation(animation), true);
            tile->addComponent<CTransform>(gridToMidPixel(x, y, tile));

            if (tile->getComponent<CAnimation>().animation.getName() == "Brick") {
                const auto& animation_size = tile->getComponent<CAnimation>().animation.getSize();
                tile->addComponent<CBBox>(animation_size);
            }
        } else if (str == "Player") {
            float x, y, bbox_w, bbox_h, v, jump_v, max_v, gravity;
            std::string bullet_animation;
            file >> x >> y >> bbox_w >> bbox_h >> v >> jump_v >> max_v >> gravity >> bullet_animation;
        } else {
            std::cerr << "Unknown level object: " << str << '\n';
        }
    }

    // auto block = m_entity_manager.addEntity("tile");
    // block->addComponent<CAnimation>(m_game_engine->assets().getAnimation("Block"), true);
    // block->addComponent<CTransform>(Vec2(224, 480));
    // auto question = m_entity_manager.addEntity("tile");
    // question->addComponent<CAnimation>(m_game_engine->assets().getAnimation("Question"), true);
    // question->addComponent<CTransform>(Vec2(352, 480));

    // NOTE: IMPORTANT:
    // Components are now returned as a references than pointers
    // If you don't specify a reference variable type, it will COPY the component:
    // Here is an example:
    //
    // This will COPY the transform into the variable 'transform1' - it's INCORRECT
    // Any changes you make to transform1 will not be changed inside the entity auto transform1 = entity->get<CTransform>()
    //
    // This will REFERENCE the transform with the variable 'transform2' - it's CORRECT
    // Now any changes made to transform2 will be changed inside the entity
    // auto& transform2 = entity->get<CTransform>()
}

void ScenePlay::spawnPlayer() {
    m_player = m_entity_manager.addEntity("player");
    m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("Stand"), true);
    m_player->addComponent<CTransform>(Vec2(224, 352));
    m_player->addComponent<CBBox>(Vec2(48, 48));
    m_player->addComponent<CGravity>(0.1f);
    // ...add also remaining components
}

void ScenePlay::spawnBullet() {
    // TODO: spawn a bullet at the given entity, going in the direction the entity is facing
    // if space bar down can CInput.canShoot becomes false, if it's up canShoot = true (only spawn bullet when canShoot = true)
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
    Vec2 player_v(0.0f, m_player->getComponent<CTransform>().velocity.y);
    if (m_player->getComponent<CInput>().up) {
        m_player->getComponent<CState>().state = "JUMP";
        player_v.y = -1;
    }

    m_player->getComponent<CTransform>().velocity = player_v;
    
    for (auto e : m_entity_manager.getEntities()){
        if (e->hasComponent<CGravity>()) {
            e->getComponent<CTransform>().velocity.y += e->getComponent<CGravity>().gravity;

            // if player is moving faster than max speed in any direction -> player speed in that direction = max speed
            // also, when landing on someting -> set player's y velocity to 0
        }
        e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;
    }

    // TODO: Implement player movement/jumping based on its CInput component
    // TODO: Implement gravity's effect on player
    // TODO: Implement the maximum player speed in both X and Y directions
    // TODO: Setting an entity's scale.x to -1/1 will make it face to the left/right
}

void ScenePlay::sLifespan() {
    // TODO: Check lifespan of entities that have them, and destroy the id they go over
}

void ScenePlay::sCollision() {
    // REMEMBER: SFML's (0, 0) position is on the TOP-LEFT corner
    // This means jumping will have a negative y-component and gravity will have positive y-component
    // Also, something BELOW something else will have a y value greater than it
    // Also, something ABOVE something else will have a y value less than it

    // TODO: Implement Physics::GetOverlap() function, use it inside this function
    // TODO: Implement bullet/tile collision (Destroy the tile if it has 'Brick' as animation)
    // TODO: Implement player/tile collision and resolutions, update the CState component of the player to
    // store whether it's currently on the ground or in the air. This will be used by the animation system.
    // TODO: Check to see if the player has fallen down a hole (y > height())
    // TODO: Don't let the player walk off the left side of the map
}

void ScenePlay::sDoAction(const Action& action) {
    if (action.getType() == "START") {
        if (action.getName() == "TOGGLE_TEXTURE") { m_draw_textures = !m_draw_textures; }
        else if (action.getName() == "TOGGLE_COLLISION") { m_draw_collision = !m_draw_collision; }
        else if (action.getName() == "TOGGLE_GRID") { m_draw_grid = !m_draw_grid; }
        else if (action.getName() == "PAUSE") { setPaused(!m_paused); }
        else if (action.getName() == "QUIT") { onEnd(); }
        else if (action.getName() == "JUMP") { m_player->getComponent<CInput>().up = true; }
        else if (action.getName() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
        else if (action.getName() == "DOWN") { m_player->getComponent<CInput>().down = true; }
        else if (action.getName() == "LEFT") { m_player->getComponent<CInput>().left = true; }
    } else if (action.getType() == "END") {
        if (action.getName() == "JUMP") { m_player->getComponent<CInput>().up = false; }
    }
    (void)action;
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
}

void ScenePlay::onEnd() {
    // TODO: When the scene ends, change back to the MENU scene
    //       use m_game_engine->ChangeScene(correctParams)
    m_engine->quit();
}
