#include "Scene.h"

void SceneMenu::init() {
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::D, "PLAY");
    registerAction(sf::Keyboard::Escape, "QUIT");
}

void SceneMenu::registerAction(int key, const std::string& action_name) {
    m_action_map[key] = action_name;
}


void SceneMenu::sDoAction(const Action& action) {
    if (action.getType() != "START") { return; }
    
    if (action.getName() == "UP") {
        if (m_selected_menu_index > 0) {
            m_selected_menu_index--;
        } else {
            
        }
    } else if (action.getName() == "DOWN") {

    } else if (action.getName() == "PLAY") {

    } else if (action.getName() == "QUIT") {

    }
}