#include "scene.hpp"

#include <filesystem>
#include <regex>

#include "../engine.hpp"
#include "../core/event.hpp"
#include "../core/key.hpp"
#include "../util/file.hpp"

SceneMenu::SceneMenu(GameEngine* engine)
    : Scene(engine) {
    init();
}

void SceneMenu::init() {
    registerAction(Up, ActionName::UP);
    registerAction(Down, ActionName::DOWN);
    registerAction(Enter, ActionName::PLAY);
    registerAction(Escape, ActionName::QUIT);

    registerAction(Event::MouseMoved, ActionName::MOUSE_MOVE);

    m_particles.init(m_engine->window().getSize());

    initMenuItems();
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
        const auto file_name = m_level_paths[m_menu_index];
        const std::string extension(std::find(file_name.begin(), file_name.end(), '.'), file_name.end());
        if (extension == ".sc.lvl") {
            m_engine->changeScene(SceneType::SIDE_SCROLL, std::make_shared<SceneSideScroller>(m_engine, file_name));
        } else if (extension == ".rpg.lvl") {
            m_engine->changeScene(SceneType::TOP_DOWN_RPG, std::make_shared<SceneRPG>(m_engine, file_name));
        } else if (m_menu_index == m_menu_strings.size() - 2) {
            files::createLevel("level" + std::to_string(m_level_paths.size() - 1), files::LevelType::SIDE_SCROLLER);
            initMenuItems();
        } else if (m_menu_index == m_menu_strings.size() - 1) {
            files::createLevel("level" + std::to_string(m_level_paths.size() - 1), files::LevelType::TOPDOWN_RPG);
            initMenuItems();
        }
    } else if (action.getName() == ActionName::QUIT) {
        onEnd();
    }
}

void SceneMenu::sRender() {
    m_engine->window().clear(Color(20, 20, 20));
    m_particles.update();
    m_particles.draw(m_engine->window());

    auto offset = 64.0f;
    size_t index = 0;
    for (const std::string& item : m_menu_strings) {
        Color color(115, 115, 115);
        if (index == m_menu_index) {
            color = Color(255, 255, 255);
        }
        renderText(item, Vec2(width()/2.0f, offset), color, true);
        offset += 32.0f;
        index++;
    }
}

void SceneMenu::onEnd() {
    m_engine->quit();
}

const std::string SceneMenu::getLevelName(const std::string& path) const {
    std::regex pattern(".*/([^/.]+)\\..*");
    std::smatch match;

    if (std::regex_match(path, match, pattern) && match.size() > 1) {
        return match[1];
    }

    return "";
}

void SceneMenu::initMenuItems() {
    m_menu_strings.clear();
    m_level_paths.clear();

    const auto path = "config/levels/";
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        m_level_paths.push_back(entry.path());
        m_menu_strings.push_back(getLevelName(entry.path()));
    }

    std::sort(std::begin(m_level_paths), std::end(m_level_paths));
    std::sort(std::begin(m_menu_strings), std::end(m_menu_strings));

    m_menu_strings.push_back("+ New side scroller level");
    m_menu_strings.push_back("+ New topdown rpg level");
    m_level_paths.push_back("");
    m_level_paths.push_back("");
}
