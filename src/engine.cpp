#include "engine.hpp"
#include "scene/scene.hpp"

#include <fstream>
#include <sstream>
#include <random>
#include <chrono>

#include <SFML/System/Clock.hpp>

#include "util/profiler.hpp"
#include "core/event.hpp"
#include "core/mouse.hpp"
#include "core/key.hpp"

GameEngine::GameEngine(const std::string& config) {
	PROFILE_FUNCTION();
	init(config);
}

void GameEngine::init(const std::string& path) {
	PROFILE_FUNCTION();

	{
		PROFILE_SCOPE("Load Assets");
		m_assets.loadFromFile(path);
	}

	{
		PROFILE_SCOPE("SFML Window");
		m_window.create(m_screen_w, m_screen_h, m_title);
		m_window.setFramerateLimit(m_framerate);
		m_window.setVerticalSyncEnabled(m_vsync);
	}

	changeScene(SceneType::MENU, std::make_shared<SceneMenu>(this));
}

void GameEngine::changeScene(SceneType scene_name, std::shared_ptr<Scene> scene, bool end_current_scene) {
	(void)end_current_scene; // TODO: Use later if needed
	m_scenes[scene_name] = scene;
	m_cur_scene = scene_name;
	m_edit_mode = false;
	m_editor.setLevel(scene->levelPath());
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
	m_editor.init(m_window.getWindow());

	while (isRunning()) {
		if (!m_paused) {
			update();
			currentScene()->update();
			// TODO: Add render() <- common function for all scenes?
		}

		if (m_edit_mode) {
			auto& entity_manager = currentScene()->getEntityManager();
			m_editor.update(m_window.getWindow(), entity_manager, this);
		}
		m_window.display();
	}
}
 
 void GameEngine::quit() {
	m_running = false;
	m_window.close();
 }

 // TODO: Re-wrap sfml functionalities use here
void GameEngine::sUserInput() {
	while (const auto event = m_window.pollEvent()) {
		m_editor.processEvent(m_window.getWindow(), *event);
		if (m_editor.windowActive()) {
			break;
		}

		if (event->is<sf::Event::Closed>()) {
			quit();
		}

		if (const auto resize = event->getIf<sf::Event::Resized>()) {
			m_window.setView(View(0, 0, resize->size.x, resize->size.y));
		}

		if (const auto key = event->getIf<sf::Event::KeyPressed>()) {
			if (key->code == sf::Keyboard::Key::F12) {
				Texture texture;
				if (texture.create(m_window.getSize().x, m_window.getSize().y)) {
					texture.update(m_window.getWindow());

					const auto t = std::time(nullptr);
					const auto tm = *std::localtime(&t);
					std::ostringstream oss;
					oss << std::put_time(&tm, "%d-%m-%Y-%H-%M-%S");
					const auto screenshot_path = "demo/screenshot-" + oss.str() + ".png";
		
					if (texture.saveToFile(screenshot_path)) {
						std::cout << "Screenshot saved to " << screenshot_path;
					}
				}
			} else {
				// if the current scene does not have an action associated with this key, skip the event
				if (currentScene()->getActionMap().find(static_cast<int>(key->code)) == currentScene()->getActionMap().end()) {
					continue;
				}
				// look up the action and send the action to the scene
				currentScene()->sDoAction(Action(currentScene()->getActionMap().at(static_cast<int>(key->code)), ActionType::START));
			}
		}

		// New action based handling
		if (const auto key = event->getIf<sf::Event::KeyReleased>()) {
			// if the current scene does not have an action associated with this key, skip the event
			if (currentScene()->getActionMap().find(static_cast<int>(key->code)) == currentScene()->getActionMap().end()) {
				continue;
			}
			// look up the action and send the action to the scene
			currentScene()->sDoAction(Action(currentScene()->getActionMap().at(static_cast<int>(key->code)), ActionType::END));
		}

		const auto pos = mouse::getPosition(m_window);

		if (const auto button = event->getIf<sf::Event::MouseButtonPressed>()) {
			switch (button->button) {
				case sf::Mouse::Button::Left: { currentScene()->sDoAction(Action(ActionName::LEFT_CLICK, ActionType::START, pos)); break; }
				case sf::Mouse::Button::Middle: { currentScene()->sDoAction(Action(ActionName::MIDDLE_CLICK, ActionType::START, pos)); break; }
				case sf::Mouse::Button::Right: { currentScene()->sDoAction(Action(ActionName::RIGHT_CLICK, ActionType::START, pos)); break; }
				default: break;
			}
		}
		if (const auto button = event->getIf<sf::Event::MouseButtonReleased>()) {
			switch (button->button) {
				case sf::Mouse::Button::Left: { currentScene()->sDoAction(Action(ActionName::LEFT_CLICK, ActionType::END, pos)); break; }
				case sf::Mouse::Button::Middle: { currentScene()->sDoAction(Action(ActionName::MIDDLE_CLICK, ActionType::END, pos)); break; }
				case sf::Mouse::Button::Right: { currentScene()->sDoAction(Action(ActionName::RIGHT_CLICK, ActionType::END, pos)); break; }
				default: break;
			}
		}
		if (event->is<sf::Event::MouseMoved>()) {
			currentScene()->sDoAction(Action(ActionName::MOUSE_MOVE, ActionType::START, pos));
		}
		if (event->is<sf::Event::MouseWheelScrolled>()) {
			if (const auto wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
				if (wheel->wheel == sf::Mouse::Wheel::Vertical && wheel->delta != 0.0f) {
					currentScene()->sDoAction(Action(ActionName::MOUSE_SCROLL, ActionType::START, wheel->delta));
				}
			}
		}
	}
}

void GameEngine::playSound(const std::string& sound_name) {
	if (!m_enable_sound) {
		return;
	}
	m_assets.getSound(sound_name).play();
}

void GameEngine::playMusic(const std::string& music_name) {
	if (!m_cur_song_name.empty()) {
		// Stop previous song
		stopMusic(m_cur_song_name);
	}
	if (!m_enable_music) {
		return;
	}

	m_cur_song_name = music_name;

	auto music = m_assets.getMusic(music_name);
	music->setVolume(25.0f);
	music->setLoop(true);
	music->play();
}

void GameEngine::toggleEditMode() {
	m_edit_mode = !m_edit_mode;
	auto entity_manager = currentScene()->getEntityManager();
	for (auto& e : entity_manager.getEntities()) {
		if (m_edit_mode) {
			e->addComponent<CDraggable>();
		} else {
			e->removeComponent<CDraggable>();
		}
	}
}

void GameEngine::stopMusic(const std::string& music_name) {
	m_assets.getMusic(music_name)->stop();
}

void GameEngine::pushSelectedPos(const Vec2& cell, bool reset) {
	if (reset) {
		m_selected_pos.clear();
		m_selected_pos.push_back(cell);
	} else if (find(m_selected_pos.begin(), m_selected_pos.end(), cell) == m_selected_pos.end()) {
		m_selected_pos.push_back(cell);
	} else if (m_selected_pos.size() > 1) {
		popSelectedPos(cell);
	}
}

void GameEngine::popSelectedPos(const Vec2& cell) {
	m_selected_pos.erase(std::remove_if(m_selected_pos.begin(), m_selected_pos.end(),
		[&](Vec2 pos) { return pos == cell; }), m_selected_pos.end());
}

void GameEngine::pushSelectedEntityId(size_t id, bool reset) {
	if (reset) {
		m_selected_entity_ids.clear();
		m_selected_entity_ids.push_back(id);
	} else if (find(m_selected_entity_ids.begin(), m_selected_entity_ids.end(), id) == m_selected_entity_ids.end()) {
		m_selected_entity_ids.push_back(id);
	} else if (m_selected_entity_ids.size() > 1) {
		popSelectedEntityId(id);
	}
}

void GameEngine::popSelectedEntityId(size_t id) {
	m_selected_entity_ids.erase(std::remove_if(m_selected_entity_ids.begin(), m_selected_entity_ids.end(),
		[&](size_t selected_id) { return selected_id == id; }), m_selected_entity_ids.end());
}

// TODO: methods below could be used directly trough Scene
const std::string& GameEngine::currentLevelPath() {
	return currentScene()->levelPath();
}

const Vec2 GameEngine::toGridPos(const Vec2& pos, const Vec2& size) {
	return currentScene()->gridPos(pos, size);
}

const PlayerConfig& GameEngine::getPlayerConfig() {
	return currentScene()->getPlayerConfig();
}

const std::vector<std::string> GameEngine::layerNames() {
	return currentScene()->getLayerNames();
}
