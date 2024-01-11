#pragma once

#include <map>
#include <memory>
#include <string>
#include "EntityManager.h"
#include "Action.h"

class GameEngine;

enum class SceneType : unsigned char {
	NONE,
	MENU,
	SIDE_SCROLL,
	TOP_DOWN_RPG
};

class Scene
{
public:
	Scene(GameEngine* engine)
		: m_engine(engine) {}

	virtual ~Scene() {}

	virtual void update() = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender() = 0;

	virtual void registerAction(int key, const ActionName action_name) { m_action_map[key] = action_name; };
	// virtual void doAction(const Action& action); ?

	size_t width() const;
	size_t height() const;
	size_t currentFrame() const { return m_current_frame; };

	bool hasEnded() const { return m_has_ended; }
	const std::map<int, ActionName>& getActionMap() const { return m_action_map; }
	void drawLine(const Vec2& p1, const Vec2& p2);

protected:
	GameEngine* m_engine{ nullptr };
	EntityManager m_entity_manager;
	size_t m_current_frame{ 0 };
	std::map<int, ActionName> m_action_map;
	bool m_paused{ false };
	bool m_has_ended{ false };

	virtual void onEnd() = 0;
	void setPaused(bool paused) { m_paused = paused; }
};

class SceneMenu: public Scene
{
	std::string m_title { "Menu" };
	std::vector<std::string> m_menu_strings{ "level 1", "level 2" };
	std::vector<std::string> m_level_paths{ "config/level1.txt", "config/level2.txt" }; // TODO: load paths from a file?
	sf::Text m_menu_text;
	sf::Color m_background_color{ 20, 20, 20 };
	size_t m_menu_index{ 0 }; // selected menu item
	unsigned int m_font_size{ 20 };

public:
	SceneMenu(GameEngine* engine);

	void init();
	void update();
	void onEnd();

	// Systems
	void sDoAction(const Action& action);
	void sRender();
};

class ScenePlay: public Scene
{
	struct PlayerConfig {
		float x{ 0.0f };
		float y{ 0.0f };
		float bbox_x{ 0.0f };
		float bbox_y{ 0.0f };
		float v{ 0.0f };
		float max_v{ 0.0f };
		float jump_v{ 0.0f };
		float gravity{ 0.0f };
		std::string weapon{ "" };
	};
	std::string m_level_path{ "" };
	std::shared_ptr<Entity> m_player;
	PlayerConfig m_player_config;
	bool m_draw_textures{ true };
	bool m_draw_collision{ false };
	bool m_draw_grid{ false };
	bool m_can_shoot{ true };
	bool m_can_jump{ true };
	const Vec2 m_grid_size{ 64, 64 };
	sf::Text m_grid_text;
	sf::Text m_pause_text;

public:
	ScenePlay(GameEngine* engine, const std::string& level_path);

	void init(const std::string& level_path);
	void update();

	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

	bool canShoot() const;
	bool canJump() const;

	void loadLevel(const std::string& path);

	void spawnPlayer();
	void spawnBullet();

	// Systems
	void sAnimation();
	void sMovement();
	void sLifespan();
	void sEnemySpawner();
	void sCollision();
	void sRender();
	void sDoAction(const Action& action);
	void sDebug();

	void onEnd();
};
