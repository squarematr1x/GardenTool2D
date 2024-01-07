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

	m_window.create(sf::VideoMode(1280, 768), "2D Game");
	m_window.setFramerateLimit(60);

	changeScene("PLAY", std::make_shared<ScenePlay>(this, "/Users/spirttipera/2D-Game-Engine/config/level.txt"));
}

void GameEngine::changeScene(const std::string& scene_name, std::shared_ptr<Scene> scene, bool end_current_scene) {
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
	}
}

// TODO: Remove this later
// void GameEngine::spawnEnemy() {
// 	static std::mt19937 mt{ static_cast<unsigned int>(
// 		std::chrono::steady_clock::now().time_since_epoch().count()
// 	) };

// 	constexpr sf::Uint32 offset { 32 };
// 	std::uniform_int_distribution rand_w{ offset, m_width - offset };
// 	std::uniform_int_distribution rand_h{ offset, m_height - offset };
// 	std::uniform_int_distribution rand_points{ m_enemy_config.VMIN, m_enemy_config.VMAX };
// 	std::uniform_int_distribution rand_rgb{ 0, 255 };

// 	std::uniform_real_distribution rand_speed{ -m_enemy_config.SMIN, m_enemy_config.SMAX };

// 	const float f_w = static_cast<float>(rand_w(mt));
// 	const float f_h = static_cast<float>(rand_h(mt));
// 	const int points = rand_points(mt);
// 	const Vec2 speed = Vec2(static_cast<float>(rand_speed(mt)), static_cast<float>(rand_speed(mt)));
// 	const auto color = sf::Color(
// 		static_cast<sf::Uint8>(rand_rgb(mt)),
// 		static_cast<sf::Uint8>(rand_rgb(mt)),
// 		static_cast<sf::Uint8>(rand_rgb(mt))
// 	);
// 	auto entity = m_entities.addEntity("enemy");

// 	entity->cShape = std::make_shared<CShape>(
// 		static_cast<float>(m_enemy_config.SR),
// 		points,
// 		color,
// 		sf::Color(
// 			static_cast<sf::Uint8>(m_enemy_config.OR),
// 			static_cast<sf::Uint8>(m_enemy_config.OG),
// 			static_cast<sf::Uint8>(m_enemy_config.OB)
// 		),
// 		static_cast<float>(m_enemy_config.OT)
// 	);
// 	entity->cTransform = std::make_shared<CTransform>(Vec2(f_w, f_h), speed, 0.0f);
// 	entity->cCollision = std::make_shared<CCollision>(static_cast<float>(m_enemy_config.CR));
// 	entity->cScore = std::make_shared<CScore>(points * 10);

// 	m_last_enemy_spawn_time = m_current_frame;
// }
