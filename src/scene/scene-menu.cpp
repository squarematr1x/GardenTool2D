#include "scene.hpp"

#include <filesystem>

#include "../engine.hpp"

SceneMenu::SceneMenu(GameEngine* engine)
    : Scene(engine) {
    init();
}

void SceneMenu::init() {
    registerAction(sf::Keyboard::Up, ActionName::UP);
    registerAction(sf::Keyboard::Down, ActionName::DOWN);
    registerAction(sf::Keyboard::Enter, ActionName::PLAY);
    registerAction(sf::Keyboard::Escape, ActionName::QUIT);

    registerAction(sf::Event::MouseMoved, ActionName::MOUSE_MOVE);

    m_menu_text.setCharacterSize(m_font_size);
    m_menu_text.setFont(m_engine->assets().getFont("Arial"));

    m_particles.init(m_engine->window().getSize());

    std::string path = "config/levels/";
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        m_level_paths.push_back(entry.path());
        m_menu_strings.push_back(entry.path());
    }

    std::sort(std::begin(m_level_paths), std::end(m_level_paths));
    std::sort(std::begin(m_menu_strings), std::end(m_menu_strings));
}

void SceneMenu::update() {
    sRender();
}

void SceneMenu::sDoAction(const Action& action) {
    if (action.getType() != ActionType::START) {
        return;
    }
    
    if (action.getName() == ActionName::MOUSE_MOVE) {
        m_particles.updateEmitter(action.pos.x, action.pos.y);
    }

    if (action.getName() == ActionName::UP) {
        if (m_menu_index > 0) {
            m_menu_index--;
        } else {
            m_menu_index = m_menu_strings.size() - 1;
        }
    } else if (action.getName() == ActionName::DOWN) {
        m_menu_index = (m_menu_index + 1) % m_menu_strings.size();
    } else if (action.getName() == ActionName::PLAY) {
        const std::string file_name = m_level_paths[m_menu_index];
        const std::string extension(std::find(file_name.begin(), file_name.end(), '.'), file_name.end());
        if (extension == ".sc.lvl") {
            m_engine->changeScene(SceneType::SIDE_SCROLL, std::make_shared<SceneSideScroller>(m_engine, file_name));
        } else if (extension == ".rpg.lvl") {
            m_engine->changeScene(SceneType::TOP_DOWN_RPG, std::make_shared<SceneRPG>(m_engine, file_name));
        }
    } else if (action.getName() == ActionName::QUIT) {
        onEnd();
    }
}

void SceneMenu::sRender() {
    m_engine->window().clear(m_background_color);
    m_particles.update();
    m_particles.draw(m_engine->window());

    float offset = 64.0f;
    size_t index = 0;
    for (const std::string& item : m_menu_strings) {
        const auto text_rect = m_menu_text.getLocalBounds();

        if (index == m_menu_index) {
            m_menu_text.setFillColor(255, 255, 255);
        } else {
            m_menu_text.setFillColor(125, 125, 125);
        }

        m_menu_text.setString(item);
        m_menu_text.setOrigin(text_rect.left + text_rect.width/2.0f, text_rect.top + text_rect.height/2.0f);
        m_menu_text.setPosition(width()/2.0f, offset);
        m_engine->window().draw(m_menu_text.getText());
        offset += 64.0f;
        index++;
    }
}

void SceneMenu::onEnd() {
    m_engine->quit();
}
