#include "GameEngine.h"
#include "Scene.h"

#include <fstream>
#include <sstream>
#include <random>
#include <chrono>

GameEngine::GameEngine(const std::string& config) {
	init(config);
}

void GameEngine::init(const std::string& path) {
	m_assets.loadFromFile(path);

	m_window.create(sf::VideoMode(m_screen_w, m_screen_h), m_title);
	m_window.setFramerateLimit(m_framerate);

	changeScene(SceneType::MENU, std::make_shared<SceneMenu>(this));
}

void GameEngine::changeScene(SceneType scene_name, std::shared_ptr<Scene> scene, bool end_current_scene) {
	(void)end_current_scene; // TODO: Use later if needed
	m_scenes[scene_name] = scene;
	m_cur_scene = scene_name;
}

void GameEngine::setPaused(bool paused) {
	m_paused = paused;
}

bool GameEngine::isRunning() {
	return m_running && m_window.isOpen();
}

void GameEngine::update() {
	sUserInput();
}

void GameEngine::run() {
	while (isRunning()) {
		if (!m_paused) {
			update();
			currentScene()->update();
		}
		m_window.display();
	}
}
 
 void GameEngine::quit() {
	m_running = false;
	m_window.close();
 }

void GameEngine::sUserInput() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			quit();
		}

		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::X) {
				sf::Texture texture;
				texture.create(m_window.getSize().x, m_window.getSize().y);
				texture.update(m_window);
				if (texture.copyToImage().saveToFile("test.png")) {
					std::cout << "Screenshot saved to " << "test.png\n";
				}
			}
		}

		// New action based handling
		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
			// if the current scene does not have an axction associated with this key, skip the event
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) { continue; }

			// determine start or end action by whether it was press or realease
			const ActionType action_type = (event.type == sf::Event::KeyPressed) ? ActionType::START : ActionType::END;

			// look up the action and send the action to the scene
			currentScene()->sDoAction(Action(currentScene()->getActionMap().at(event.key.code), action_type));
		}

		auto mouse_pos = sf::Mouse::getPosition(m_window);
		Vec2 pos(mouse_pos.x, mouse_pos.y);

		if (event.type == sf::Event::MouseButtonPressed) {
			switch (event.mouseButton.button) {
				case sf::Mouse::Left: { currentScene()->sDoAction(Action(ActionName::LEFT_CLICK, ActionType::START, pos)); break; }
				case sf::Mouse::Middle: { currentScene()->sDoAction(Action(ActionName::MIDDLE_CLICK, ActionType::START, pos)); break; }
				case sf::Mouse::Right: { currentScene()->sDoAction(Action(ActionName::RIGHT_CLICK, ActionType::START, pos)); break; }
				default: break;
			}
		}
		if (event.type == sf::Event::MouseButtonReleased) {
			switch (event.mouseButton.button) {
				case sf::Mouse::Left: { currentScene()->sDoAction(Action(ActionName::LEFT_CLICK, ActionType::END, pos)); break; }
				case sf::Mouse::Middle: { currentScene()->sDoAction(Action(ActionName::MIDDLE_CLICK, ActionType::END, pos)); break; }
				case sf::Mouse::Right: { currentScene()->sDoAction(Action(ActionName::RIGHT_CLICK, ActionType::END, pos)); break; }
				default: break;
			}
		}
		if (event.type == sf::Event::MouseMoved) {
			currentScene()->sDoAction(Action(ActionName::MOUSE_MOVE, ActionType::START, pos));
		}
	}
}
