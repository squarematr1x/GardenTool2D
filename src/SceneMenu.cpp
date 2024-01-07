#include "Scene.h"
#include "GameEngine.h"

void SceneMenu::init() {
    registerAction(sf::Keyboard::Up, ActionName::UP);
    registerAction(sf::Keyboard::Down, ActionName::DOWN);
    registerAction(sf::Keyboard::Enter, ActionName::PLAY);
    registerAction(sf::Keyboard::Escape, ActionName::QUIT);
}

void SceneMenu::update() {
    // sDoAction();
    sRender();
}

void SceneMenu::sDoAction(const Action& action) {
    if (action.getType() != ActionType::START) { return; }
    
    if (action.getName() == ActionName::UP) {
        if (m_menu_index > 0) { m_menu_index--; } 
        else { m_menu_index = m_menu_strings.size() - 1; }
    } else if (action.getName() == ActionName::DOWN) {
        m_menu_index = (m_menu_index + 1) % m_menu_strings.size();
    } else if (action.getName() == ActionName::PLAY) {
        m_engine->changeScene("PLAY", std::make_shared<ScenePlay>(m_engine, m_level_paths[m_menu_index]));
    } else if (action.getName() == ActionName::QUIT) {
        onEnd();
    }
}

void SceneMenu::sRender() {
    // TODO: Render menu here
}

void SceneMenu::onEnd() {
    // Close the game
    m_engine->quit();
}
