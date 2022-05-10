#include "Game.h"

#include <fstream>
#include <sstream>
#include <random>
#include <chrono>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& path)
{
	std::ifstream file_in{ path };

	if (!file_in)
	{
		std::cerr << "Config file not found\n";
		exit(-1);
	}

	auto screen_mode{ sf::Style::Resize | sf::Style::Close};
	bool full_screen{ false };

	while (file_in)
	{
		std::string line;
		std::getline(file_in, line);
		std::stringstream ssin(line);
		std::vector<std::string> args;

		while (ssin.good())
		{
			std::string arg;
			ssin >> arg;
			args.push_back(arg);

			if (arg == "Window")
			{
				int fps{ 60 };
				ssin >> m_width >> m_height >> fps >> full_screen;

				if (full_screen)
					screen_mode = sf::Style::Fullscreen;
			}
			else if (arg == "Player")
			{
				ssin >> m_player_config.SR
					>> m_player_config.CR
					>> m_player_config.S
					>> m_player_config.FR
					>> m_player_config.FG
					>> m_player_config.FB
					>> m_player_config.OR
					>> m_player_config.OG
					>> m_player_config.OB
					>> m_player_config.OT
					>> m_player_config.V;
			}
		}

		if (args[0] == "Window")
		{
			//m_width = stoi(args[1]);
			//m_height = stoi(args[2]);
			//bool full_screen{ false };
			//std::istringstream(args[4]) >> full_screen;

			//if (full_screen)
			//	screen_mode = sf::Style::Fullscreen;
		}
		else if (args[0] == "Font")
		{

		}
		else if (args[0] == "Player")
		{

		}
		else if (args[0] == "Enemy")
		{

		}
		else if (args[0] == "Bullet")
		{

		}
	}


	if (!m_font.loadFromFile("fonts/arial.ttf"))
	{
	    std::cerr << "Font not found\n";
	    exit(-1);
	}
	m_text.setFont(m_font);
	m_text.setCharacterSize(24);
	m_text.setPosition(32, 32);

	m_window.create(sf::VideoMode(m_width, m_height), "2D Game Engine", screen_mode);
	m_window.setFramerateLimit(60);

	spawnPlayer();
}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::run()
{
	while (m_running)
	{
		m_entities.update();

		if (!m_paused)
		{
			sEnemySpawner();
			sMovement();
			sLifeSpan();
		}

		sCollision();
		sUserInput();
		sRender();

		m_current_frame++;
	}
}


void Game::spawnPlayer()
{
	auto entity = m_entities.addEntity("player");

	// Use struct read from config file when ready

	entity->cTransform = std::make_shared<CTransform>(Vec2(m_width/2, m_height/2), Vec2(0.0f, -1.0f), 0.0f);
	entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);
	entity->cInput = std::make_shared<CInput>();
	entity->cCollision = std::make_shared<CCollision>(32.0f);

	m_player = entity;
}

// Spawn an enemy at a random position
void Game::spawnEnemy()
{
	static std::mt19937 mt{ static_cast<unsigned int>(
		std::chrono::steady_clock::now().time_since_epoch().count()
	) };

	std::uniform_int_distribution rand_w{ 32, m_width - 32 };
	std::uniform_int_distribution rand_h{ 32, m_height - 32 };
	std::uniform_int_distribution rand_points{ 3, 8 };

	const float f_w = static_cast<float>(rand_w(mt));
	const float f_h = static_cast<float>(rand_h(mt));
	const int points = rand_points(mt);

	auto entity = m_entities.addEntity("enemy");
	entity->cTransform = std::make_shared<CTransform>(Vec2(f_w, f_h), Vec2(0.5f, 1.0f), 0.0f);
	entity->cShape = std::make_shared<CShape>(32.0f, points, sf::Color(10, 10, 10), sf::Color(255, 255, 255), 4.0f);
	entity->cCollision = std::make_shared<CCollision>(32.0f);
	entity->cScore = std::make_shared<CScore>(points * 10);

	m_last_enemy_spawn_time = m_current_frame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	const int vertices = entity->cShape->circle.getPointCount();
	int step = 360 / vertices;

	for (int i = 1; i <= vertices; i++)
	{
		auto small_enemy = m_entities.addEntity("small");
		small_enemy->cScore = std::make_shared<CScore>(entity->cScore->score * 2);
		small_enemy->cShape = std::make_shared<CShape>(10.0f, entity->cShape->circle.getPointCount(), entity->cShape->circle.getFillColor(), entity->cShape->circle.getOutlineColor(), 4.0f);
		Vec2 speed(cosf(static_cast<float>(i * step) * 3.141592f / 180.0f), sinf(static_cast<float>(i * step) * 3.141592f / 180.0f));
		small_enemy->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, speed , 0.0f);
		small_enemy->cCollision = std::make_shared<CCollision>(10.0f);
		small_enemy->cLifespan = std::make_shared<CLifespan>(120);
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mouse_pos)
{
	auto bullet = m_entities.addEntity("bullet");
	Vec2 diff = mouse_pos - entity->cTransform->pos;
	bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, diff.normalize()*2.5f, 0.0f);
	bullet->cShape = std::make_shared<CShape>(10.0f, 32, sf::Color(200, 200, 200), sf::Color(200, 200, 200), 4.0f);
	bullet->cCollision = std::make_shared<CCollision>(16.0f);
	bullet->cLifespan = std::make_shared<CLifespan>(120);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	if (m_current_frame - m_last_special_weapon_time < 300)
		return;

	const int vertices = entity->cShape->circle.getPointCount();
	int step = 360 / vertices;

	for (int i = 1; i <= vertices; i++)
	{
		auto small_bullet = m_entities.addEntity("bullet");
		small_bullet->cShape = std::make_shared<CShape>(10.0f, 16, sf::Color(150, 150, 150), sf::Color(150, 150, 150), 4.0f);
		Vec2 speed(cosf(static_cast<float>(i * step) * 3.141592f / 180.0f), sinf(static_cast<float>(i * step) * 3.141592f / 180.0f));
		small_bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, speed, 0.0f);
		small_bullet->cCollision = std::make_shared<CCollision>(10.0f);
		small_bullet->cLifespan = std::make_shared<CLifespan>(150);
	}

	m_last_special_weapon_time = m_current_frame;
}

void Game::sMovement()
{
	Vec2 player_velocity(0.0f, 0.0f);

	if (m_player->cInput->left)
	{
		player_velocity.x -= 5.0f;
	}
	if (m_player->cInput->right)
	{
		player_velocity.x += 5.0f;
	}
	if (m_player->cInput->up)
	{
		player_velocity.y -= 5.0f;
	}
	if (m_player->cInput->down)
	{
		player_velocity.y += 5.0f;
	}

	m_player->cTransform->velocity = player_velocity;

	for (auto& entity : m_entities.getEntities())
	{
		float speed = 1.0f;

		if (entity->tag() == "enemy")
			speed = -1.0f;

		if (entity->cTransform->pos.y - entity->cShape->circle.getRadius() < 0)
			entity->cTransform->velocity.y *= speed;
		else if (entity->cTransform->pos.y + entity->cShape->circle.getRadius() > m_height)
			entity->cTransform->velocity.y *= speed;
		else if (entity->cTransform->pos.x - entity->cShape->circle.getRadius() < 0)
			entity->cTransform->velocity.x *= speed;
		else if (entity->cTransform->pos.x + entity->cShape->circle.getRadius() > m_width)
			entity->cTransform->velocity.x *= speed;

		entity->cTransform->pos += entity->cTransform->velocity;
	}
}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = true;
				break;
			case sf::Keyboard::P:
				setPaused(!m_paused);
				break;
			case sf::Keyboard::Escape:
				m_running = false;
				break;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			Vec2 mouse_pos = Vec2{ event.mouseButton.x, event.mouseButton.y };

			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left:
				spawnBullet(m_player, mouse_pos);
				break;
			case sf::Mouse::Right:
				spawnSpecialWeapon(m_player);
				break;
			}
		}
	}
}

void Game::sLifeSpan()
{
	for (auto& entity : m_entities.getEntities())
	{
		if (entity->cLifespan)
		{
			if (entity->cLifespan->remaining == 0)
				entity->destroy();
			else
			{
				auto& color = entity->cShape->circle.getFillColor();
				auto& outline = entity->cShape->circle.getOutlineColor();
				sf::Uint8 new_alpha = static_cast<sf::Uint8>((static_cast<float>(entity->cLifespan->remaining) / entity->cLifespan->total) * 255);
				sf::Color new_color(color.r, color.g, color.b, new_alpha);
				sf::Color new_outline(outline.r, outline.g, outline.b, new_alpha);
				entity->cShape->circle.setFillColor(new_color);
				entity->cShape->circle.setOutlineColor(new_outline);
				entity->cLifespan->remaining -= 1;
			}
		}
	}
}

void Game::sRender()
{
	m_window.clear();
	
	for (auto& entity : m_entities.getEntities())
	{
		entity->cShape->circle.setPosition(entity->cTransform->pos.x, entity->cTransform->pos.y);
		entity->cTransform->angle += 1.0f;
		entity->cShape->circle.setRotation(entity->cTransform->angle);
		m_window.draw(entity->cShape->circle);
	}

	m_text.setString("Score: " + std::to_string(m_score));
	m_window.draw(m_text);
	m_window.display();
}

void Game::sEnemySpawner()
{
	if (m_current_frame - m_last_enemy_spawn_time > 200)
		spawnEnemy();
}

void Game::sCollision()
{
	for (auto& player : m_entities.getEntities("player"))
	{
		for (auto& enemy : m_entities.getEntities("enemy"))
		{
			if (isCollision(player, enemy))
			{
				player->destroy();
				spawnPlayer();
			}
		}
	}

	for (auto& bullet : m_entities.getEntities("bullet"))
	{
		for (auto& enemy : m_entities.getEntities("enemy"))
		{
			if (isCollision(bullet, enemy))
			{
				enemy->destroy();
				bullet->destroy();
				spawnSmallEnemies(enemy);
				m_score += enemy->cScore->score;
			}
		}
		for (auto& small_enemy : m_entities.getEntities("small"))
		{
			if (isCollision(bullet, small_enemy))
			{
				small_enemy->destroy();
				bullet->destroy();
				m_score += small_enemy->cScore->score;
			}
		}
	}
}

bool Game::isCollision(const std::shared_ptr<Entity>& entity_a, const std::shared_ptr<Entity>& entity_b) const
{
	float distance = entity_a->cTransform->pos.distance(entity_b->cTransform->pos);
	float entity_a_r = entity_a->cCollision->radius;
	float entity_b_r = entity_b->cCollision->radius;

	return (distance < entity_a_r + entity_b_r);
}
