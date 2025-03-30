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

void GameEngine::sUserInput() {
	Event event;

	while (m_window.pollEvent(event)) {
		m_editor.processEvent(m_window.getWindow(), event.getEvent());
		if (m_editor.windowActive()) {
			break;
		}

		if (event.getType() == Event::Closed) {
			quit();
		}

		if (event.getType() == Event::Resized) {
			m_window.setView(View(0, 0, event.getWidth(), event.getHeight()));
		}

		if (event.getType() == Event::KeyPressed) {
			if (event.getKeyCode() == F12) {
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
			}
		}

		// New action based handling
		if (event.getType() == Event::KeyPressed || event.getType() == Event::KeyReleased) {
			// if the current scene does not have an action associated with this key, skip the event
			if (currentScene()->getActionMap().find(event.getKeyCode()) == currentScene()->getActionMap().end()) {
				continue;
			}

			// determine start or end action by whether it was press or realease
			const auto action_type = (event.getType() == Event::KeyPressed) ? ActionType::START : ActionType::END;

			// look up the action and send the action to the scene
			currentScene()->sDoAction(Action(currentScene()->getActionMap().at(event.getKeyCode()), action_type));
		}

		const auto pos = mouse::getPosition(m_window);

		if (event.getType() == Event::MouseButtonPressed) {
			switch (event.getMouseButton()) {
				case mouse::Left: { currentScene()->sDoAction(Action(ActionName::LEFT_CLICK, ActionType::START, pos)); break; }
				case mouse::Middle: { currentScene()->sDoAction(Action(ActionName::MIDDLE_CLICK, ActionType::START, pos)); break; }
				case mouse::Right: { currentScene()->sDoAction(Action(ActionName::RIGHT_CLICK, ActionType::START, pos)); break; }
				default: break;
			}
		}
		if (event.getType() == Event::MouseButtonReleased) {
			switch (event.getMouseButton()) {
				case mouse::Left: { currentScene()->sDoAction(Action(ActionName::LEFT_CLICK, ActionType::END, pos)); break; }
				case mouse::Middle: { currentScene()->sDoAction(Action(ActionName::MIDDLE_CLICK, ActionType::END, pos)); break; }
				case mouse::Right: { currentScene()->sDoAction(Action(ActionName::RIGHT_CLICK, ActionType::END, pos)); break; }
				default: break;
			}
		}
		if (event.getType() == Event::MouseMoved) {
			currentScene()->sDoAction(Action(ActionName::MOUSE_MOVE, ActionType::START, pos));
		}
		if (event.getType() == Event::MouseWheelScrolled) {
			if (event.getMouseWheelScrollWheel() == mouse::VerticalWheel) {
				if (event.getMouseWheelScrollDelta() != 0.0f) {
					currentScene()->sDoAction(Action(ActionName::MOUSE_SCROLL, ActionType::START, event.getMouseWheelScrollDelta()));
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

// TODO: methods below could be used directly trough Scene
const std::string& GameEngine::currentLevelPath() {
	return currentScene()->levelPath();
}

const Vec2 GameEngine::toGridPos(const Vec2& pos) {
	return currentScene()->gridPos(pos);
}

const PlayerConfig& GameEngine::getPlayerConfig() {
	return currentScene()->getPlayerConfig();
}

const std::vector<std::string> GameEngine::layerNames() {
	return currentScene()->getLayerNames();
}
