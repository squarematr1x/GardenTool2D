#include "GameEngine.h"

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

	changeScene("MENU", std::make_shared<SceneMenu>(this));

	// TODO: remove the stuff below
	// std::ifstream file_in{ path };

	// if (!file_in)
	// {
	// 	std::cerr << "Config file not found\n";
	// 	exit(-1);
	// }

	// auto screen_mode{ sf::Style::Resize | sf::Style::Close};
	// bool full_screen{ false };

	// // FIXME: Make a better config file parser and simplify config file structure
	// while (file_in)
	// {
	// 	std::string line;
	// 	std::getline(file_in, line);
	// 	std::stringstream ssin(line);
	// 	std::vector<std::string> args;

	// 	while (ssin.good())
	// 	{
	// 		std::string arg;
	// 		ssin >> arg;
	// 		args.push_back(arg);

	// 		if (arg == "Window")
	// 		{
	// 			int fps{ 60 };
	// 			ssin >> m_width >> m_height >> fps >> full_screen;

	// 			if (full_screen)
	// 				screen_mode = sf::Style::Fullscreen;
	// 		}
	// 		else if (arg == "Font")
	// 		{
	// 			std::string font_file;
	// 			sf::Uint8 font_size;
	// 			sf::Uint8 font_R;
	// 			sf::Uint8 font_G;
	// 			sf::Uint8 font_B;
	// 			ssin >> font_file >> font_size >> font_R >> font_G >> font_B;

	// 			if (!m_font.loadFromFile(font_file))
	// 			{
	// 				std::cerr << "Font not found\n";
	// 				exit(-1);
	// 			}

	// 			m_text.setFont(m_font);
	// 			m_text.setCharacterSize(font_size);
	// 			m_text.setFillColor(sf::Color(font_R, font_G, font_B));
	// 			m_text.setPosition(32, 32);
	// 		}
	// 		else if (arg == "Player")
	// 		{
	// 			ssin >> m_player_config.SR
	// 				>> m_player_config.CR
	// 				>> m_player_config.S
	// 				>> m_player_config.FR
	// 				>> m_player_config.FG
	// 				>> m_player_config.FB
	// 				>> m_player_config.OR
	// 				>> m_player_config.OG
	// 				>> m_player_config.OB
	// 				>> m_player_config.OT
	// 				>> m_player_config.V;
	// 		}
	// 		else if (arg == "Enemy")
	// 		{
	// 			ssin >> m_enemy_config.SR
	// 				>> m_enemy_config.CR
	// 				>> m_enemy_config.SMIN
	// 				>> m_enemy_config.SMAX
	// 				>> m_enemy_config.OR
	// 				>> m_enemy_config.OG
	// 				>> m_enemy_config.OB
	// 				>> m_enemy_config.OT
	// 				>> m_enemy_config.VMIN
	// 				>> m_enemy_config.VMAX
	// 				>> m_enemy_config.L
	// 				>> m_enemy_config.SI;
	// 		}
	// 		else if (arg == "Bullet")
	// 		{
	// 			ssin >> m_bullet_config.SR
	// 				>> m_bullet_config.CR
	// 				>> m_bullet_config.S
	// 				>> m_bullet_config.FR
	// 				>> m_bullet_config.FG
	// 				>> m_bullet_config.FB
	// 				>> m_bullet_config.OR
	// 				>> m_bullet_config.OG
	// 				>> m_bullet_config.OB
	// 				>> m_bullet_config.OT
	// 				>> m_bullet_config.V
	// 				>> m_bullet_config.L;
	// 		}
	// 	}
	// }

	// m_window.create(sf::VideoMode(m_width, m_height), "2D Game Engine", static_cast<sf::Uint32>(screen_mode));
	// m_window.setFramerateLimit(60);

	// spawnPlayer();
}

void GameEngine::setPaused(bool paused) {
	m_paused = paused;
}

bool GameEngine::isRunning() {
	return m_running && m_window.isOpen();
}

void GameEngine::run() {
	while (isRunning()) {
		if (!m_paused) {
			currentScene()->update();
		}
	}
}

void GameEngine::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			m_running = false; // TODO: Maybe remove this also
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
		if (event.type == sf::Event::KeyPressed || sf::Event::KeyReleased) {
			// if the current scene does not have an axction associated with this key, skip the event
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) { continue; }

			// determine start or end action by whether it was press or realease
			const std::string action_type = (event.type == sf::Event::KeyPressed) ? "START" : "END";

			// look up the action and send the action to the scene
			currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), action_type));
		}
	}
}

// TODO: Remove this later
// void GameEngine::sRender() {
// 	m_window.clear();
	
// 	for (auto& entity : m_entities.getEntities())
// 	{
// 		entity->cShape->circle.setPosition(entity->cTransform->pos.x, entity->cTransform->pos.y);
// 		entity->cTransform->angle += 1.0f;
// 		entity->cShape->circle.setRotation(entity->cTransform->angle);
// 		m_window.draw(entity->cShape->circle);
// 	}

// 	m_text.setString("Score: " + std::to_string(m_score));
// 	m_window.draw(m_text);
// 	m_window.display();
// }

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
