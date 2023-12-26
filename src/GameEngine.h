#pragma once

#include "EntityManager.h"
// #include "Scene.h"
// #include "Asset.h"

struct PlayerConfig {
	int SR, CR, OT, V, FR, FG, FB, OR, OG, OB;
	float S;
};

struct EnemyConfig {
	int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
	float SMIN, SMAX;
};
struct BulletConfig {
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
	float S;
};

class GameEngine
{
public:
	GameEngine(const std::string& config);

	void run();
	// void changeScene(const std::string& scene);
	// std::vector<Asset> getAssets();
	// std::shared_ptr<Scene> currentScene() { return m_scenes[m_cur_scene]; }

private:
	sf::RenderWindow m_window;
	sf::Font m_font;
	sf::Text m_text;
	EntityManager m_entities;
	PlayerConfig m_player_config;
	EnemyConfig m_enemy_config;
	BulletConfig m_bullet_config;
	int m_score{ 0 };
	int m_current_frame{ 0 };
	int m_last_enemy_spawn_time{ 0 };
	int m_last_special_weapon_time{ 0 };
	sf::Uint32 m_width{ 0 };
	sf::Uint32 m_height{ 0 };
	bool m_paused{ false };
	bool m_running{ true };

	std::shared_ptr<Entity> m_player;

	// std::map<std::string, std::shared_ptr<Scene>> m_scenes;
	// std::string m_cur_scene;
	// std::vector<Asset> assets;

	bool isCollision(const std::shared_ptr<Entity>& entity_a, const std::shared_ptr<Entity>& entity_b) const;

	void init(const std::string& config);
	void setPaused(bool paused);

	void sMovement();
	void sUserInput();
	void sLifeSpan();
	void sRender();
	void sEnemySpawner();
	void sCollision();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mouse_pos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);
};
