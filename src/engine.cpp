#include "engine.hpp"
#include "scene/scene.hpp"

#include <fstream>
#include <sstream>
#include <random>
#include <chrono>

#include <SFML/System/Clock.hpp>

#include "../vendor/imgui.h"
#include "../vendor/imgui-SFML.h"

#include "util/profiler.hpp"

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
		m_window.create(sf::VideoMode(m_screen_w, m_screen_h), m_title);
		m_window.setFramerateLimit(m_framerate);
	}

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
	sf::Clock dt;
	bool init = ImGui::SFML::Init(m_window);
	(void)init;

	while (isRunning()) {
		if (!m_paused) {
			update();
			currentScene()->update();
			// TODO: Add render() <- common function for all scenes?
		}
		ImGui::SFML::Update(m_window, dt.restart());
		ImGui::ShowDemoWindow();

		ImGui::Begin("Hello, world!");
		ImGui::Button("Look at this pretty button");
		ImGui::End();

		ImGui::SFML::Render(m_window);
		m_window.display();
	}
	ImGui::SFML::Shutdown();
}
 
 void GameEngine::quit() {
	m_running = false;
	m_window.close();
 }

void GameEngine::sUserInput() {
	sf::Event event;

	while (m_window.pollEvent(event)) {
		ImGui::SFML::ProcessEvent(m_window, event);

		if (event.type == sf::Event::Closed) {
			quit();
		}

		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::F12) {
				sf::Texture texture;
				texture.create(m_window.getSize().x, m_window.getSize().y);
				texture.update(m_window);

				auto t = std::time(nullptr);
				auto tm = *std::localtime(&t);
				std::ostringstream oss;
				oss << std::put_time(&tm, "%d-%m-%Y-%H-%M-%S");
				auto screenshot_path = "demo/screenshot-" + oss.str() + ".png";
	
				if (texture.copyToImage().saveToFile(screenshot_path)) {
					std::cout << "Screenshot saved to " << screenshot_path;
				}
			}
		}

		// New action based handling
		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
			// if the current scene does not have an action associated with this key, skip the event
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) {
				continue;
			}

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
		if (event.type == sf::Event::MouseWheelScrolled) {
			if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
				if (event.mouseWheelScroll.delta != 0.0f) {
					currentScene()->sDoAction(Action(ActionName::MOUSE_SCROLL, ActionType::START, event.mouseWheelScroll.delta));
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

void GameEngine::stopMusic(const std::string& music_name) {
	m_assets.getMusic(music_name)->stop();
}
