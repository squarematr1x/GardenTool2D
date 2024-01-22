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
    (void)path;
    // TODO: load level from file
    spawnPlayer();
}

Vec2 SceneRPG::getPosition(int rx, int ry, int tx, int ty) const {
    (void)rx;
    (void)ry;
    (void)tx;
    (void)ty;
    return Vec2(0, 0);
}

void SceneRPG::spawnPlayer() {
    m_player = m_entity_manager.addEntity(Tag::PLAYER);
    m_player->addComponent<CTransform>(Vec2(640, 480));
    m_player->addComponent<CAnimation>(m_engine->assets().getAnimation("StandDown"), true);
    m_player->addComponent<CBBox>(Vec2(48, 48), true, false);
    m_player->addComponent<CHealth>(7, 3);
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

    m_current_frame++;
}

void SceneRPG::sMovement() {
    // TODO: Implement player movement functionality based on input
}

void SceneRPG::sDoAction(const Action& action) {
    // TODO: Setting of the player's input component variables only
    //       use minimalistic logic

    if (action.getType() == ActionType::START) {
        switch (action.getName()) {
            case ActionName::PAUSE: break;
            case ActionName::QUIT: break;
            case ActionName::TOGGLE_FOLLOW: m_follow = !m_follow; break;
            case ActionName::TOGGLE_TEXTURE: break;
            case ActionName::TOGGLE_COLLISION: break;
            default: break;
        }
    } else if (action.getType() == ActionType::END) {

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

