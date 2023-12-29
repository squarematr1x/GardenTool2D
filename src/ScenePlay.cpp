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

    // spawnPlayer();

    auto brick = m_entity_manager.addEntity("tile");
    // IMPORTANT: always add the CAnimation component first that gridToMidPixel can compute correctly 54:43
    //brick->addCo

}

void ScenePlay::sDoAction(const Action& action) {
    (void)action;
}
