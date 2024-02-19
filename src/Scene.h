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

	// Rendering helpers
	void drawLine(const Vec2& p1, const Vec2& p2);

	void renderGrid(const Vec2& grid_size, sf::Text& grid_text);
	void renderBBoxes();
	void renderCursor();
	void renderHealth(std::shared_ptr<Entity> e);

protected:
	GameEngine* m_engine{ nullptr };
	EntityManager m_entity_manager;
	size_t m_current_frame{ 0 };
	std::map<int, ActionName> m_action_map;
	bool m_paused{ false };
	bool m_has_ended{ false };
	bool m_draw_textures{ true };
	bool m_draw_collision{ false };
	bool m_draw_grid{ false };

	virtual void onEnd() = 0;
	void setPaused(bool paused) { m_paused = paused; }
};

class SceneMenu: public Scene
{
	std::string m_title { "Menu" };
	std::vector<std::string> m_menu_strings{ "level 1", "level 2", "level 3" };
	std::vector<std::string> m_level_paths{
		"config/levels/level1.lvl",
		"config/levels/level2.lvl",
		"config/levels/level3.lvl",
	}; // TODO: load paths from a file?
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

class SceneSideScroller: public Scene
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
	bool m_can_shoot{ true };
	bool m_can_jump{ true };
	const Vec2 m_grid_size{ 64, 64 };
	sf::Text m_grid_text;
	sf::Text m_pause_text;

	Vec2 m_mouse_pos;
	sf::CircleShape m_mouse_shape;

public:
	SceneSideScroller(GameEngine* engine, const std::string& level_path);

	void init(const std::string& level_path);
	void update();

	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
	Vec2 mouseToWorldPos(const Vec2& mouse_pos) const;

	bool canShoot() const;
	bool canJump() const;

	void loadLevel(const std::string& path);

	void spawnPlayer();
	void spawnBullet();
	void spawnExplosion(const Vec2& pos);

	// Systems
	void sAnimation();
	void sMovement();
	void sLifespan();
	void sEnemySpawner();
	void sCollision();
	void sCamera();
	void sRender();
	void sDoAction(const Action& action);
	void sDragAndDrop();
	void sDebug();

	void onEnd();
};

class SceneRPG: public Scene
{
	struct PlayerConfig {
		float x{ 0.0f };
		float y{ 0.0f };
		float bbox_x{ 0.0f };
		float bbox_y{ 0.0f };
		float v{ 0.0f };
		int health{ 4 };
	};
	PlayerConfig m_player_config;
	std::string m_level_path{ "" };
	std::shared_ptr<Entity> m_player;
	bool m_follow{ false };
	bool m_show_health{ true };
	const Vec2 m_grid_size{ 64, 64 };
	const Vec2 m_room_size{ 20, 12 }; // 20x12 grids

public:
	SceneRPG(GameEngine* engine, const std::string& level_path);

	void init(const std::string& level_path);
	void update();

	void loadLevel(const std::string& path);

	void spawnPlayer();
	void spawnSword(std::shared_ptr<Entity> entity);

	void teleport();

	Vec2 getPosition(float rx, float ry, float tx, float ty) const;
	Vec2 getCurrentRoom() const;

	bool targetReached(const Vec2& pos, const Vec2& target) const;

	void sAI();
	void sMovement();
	void sStatus();
	void sCollision();
	void sAnimation();
	void sCamera();
	void sDoAction(const Action& action);
	void sRender();

	void onEnd();
};
