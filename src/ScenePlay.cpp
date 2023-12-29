#include "Scene.h"

ScenePlay::ScenePlay(GameEngine* game_engine, const std::string& level_path)
    : Scene(game_engine), m_level_path(level_path) {
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

    m_grid_text.setCharacterSize(12);
    
    //m_grid_text.setFont(m_game_engine->assets().getFont("Tech"));
}

Vec2 ScenePlay::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) {
    // TODO: This function takes grid x,y position and an Entity
    // returns a Vec2 indicatinh where the CENTER position of the Entity should be
    // You must use the Entity's Animation size to position it correctly
    // The size of the grid width and height is stored in m_grid_size.x and m_grid_size.y
    // The bottom-left corner of the Animation should align with the bottom left of the grid cell
    return Vec2(0, 0);
}

void ScenePlay::loadLevel(const std::string& path) {
    // resset the entity manager every time we load a level
    m_entity_manager = EntityManager();

    // TODO: read in the level file and add the appropriate entities
    // use the PlayerConfig struct m_playerConfig to store player properties
    // this struct is defined at the top of ScenePlay.h

    // NOTE: all of thre code below is sample code which shows you how to set up and use entitties with the new syntax

    spawnPlayer();

    auto brick = m_entity_manager.addEntity("tile");
    // IMPORTANT: always add the CAnimation component first that gridToMidPixel can compute correctly 54:43
    brick->cAnimation = std::make_shared<CAnimation>(m_game_engine->assets().getAnimation("Block"), true);
    brick->cTransform = std::make_shared<CTransform>(Vec2(224, 480));
    // NOTE: You final code should position the entity with the grid x,y position read from
    // brick->addComponent<CTransform>(gridToMidPixel(grid_x, grid_y, brick));

    if (brick->getComponent<CAnimation>().animation_name == "Brick") {
        std::cout << "This could be good identifying whether a tile is brick\n";
    }

    auto block = m_entity_manager.addEntity("tile");
    brick->cAnimation = std::make_shared<CAnimation>(m_game_engine->assets().getAnimation("Question"), true);
    brick->cTransform = std::make_shared<CTransform>(Vec2(352, 480));

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
    // here is a sample entity which you can use to construct other entities
    m_player = m_entity_manager.addEntity("player");
    // TODO: Implement addComponent<T>() method for entity class
    // e.g.
    // m_player->addComponent<CAnimation>(m_game_engine->assets().getAnimation("Stand"), true);
    // m_player->addComponent<CTransform>(Vec2(224, 352));
    // m_player->addComponent<CBBox>(Vec2(48, 48));
    // ...add also remaining components
}

void ScenePlay::spawnBullet() {
    // TODO: spawn a bullet at the given entity, going in the direction the entity is facing
}

void ScenePlay::update() {
    m_entity_manager.update();

    // TODO: implement pause functionality

    sMovement();
    sLifespan();
    sCollision();
    sAnimation();
    sRender();
}

void ScenePlay::sMovement() {
    // TODO: Implement player movement/jumping based on its CInput component
    // TODO: Implement gravity's effect on player
    // TODO: Implement the maximum player speed in both X and Y directions
    // TODO: Setting an entity's scale.x to -1/1 will make it face to the left/right
}

void ScenePlay::sLifeSpan() {
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
        else if (action.getName() == "JUMP") { /* Jump */ }
    } else if (action.getType() == "END") {

    }
    (void)action;
}
