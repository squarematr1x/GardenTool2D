#pragma once

#include <map>
#include <memory>
#include <string>

#include "../ecs/entity-manager.hpp"
#include "../action.hpp"
#include "../particle-system.hpp"
#include "../layer.hpp"
#include "../player-config.hpp"

#include "../asset/text.hpp"

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
	// To zoom views when mouse scrolled
	struct Zoom {
		float value{ 1.0f }; // For setting the view
		float magnitude{ 0.0f }; // For world pos calculations
		int level{ 0 };
		int prev_level{ 0 };
		int max_level{ 4 };
	};

	virtual ~Scene() {}

	virtual void update() = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender() = 0;

	virtual void registerAction(int key, const ActionName action_name) { m_action_map[key] = action_name; };
	// virtual void doAction(const Action& action); ?

	unsigned int width() const;
	unsigned int height() const;
	size_t currentFrame() const { return m_current_frame; };

	bool hasEnded() const { return m_has_ended; }
	const std::map<int, ActionName>& getActionMap() const { return m_action_map; }

	Vec2 gridPos(const Vec2& pos) const;
	Vec2 fitToGrid(const Vec2& pos, bool mid_pixel = true) const;
	Vec2 worldPos();
	Vec2 getCenter() const;

	// Rendering helpers
	void drawLine(const Vec2& p1, const Vec2& p2);

	void renderGrid(bool show_coordinates = false);
	void renderActiveGridCell();
	void renderSelectedGridCell();
	void renderBBoxes();
	void renderCursor();
	void renderHpBars();
	void renderHighlights();
	void renderInfoAI(std::shared_ptr<Entity> e, std::shared_ptr<Entity> player);
	void renderPauseText();
	void renderCommon(std::shared_ptr<Entity> player);

	void updateZoom(float delta);
	void updateGridSize(Vec2 size) { m_grid_size = size; }

	void addVertexData(const Vec2& pos, const sf::IntRect& texture_rect_in, VertexArray& vertices);
	void addLine(const Vec2& p1, const Vec2& p2, VertexArray& vertices);
	void addHpBar(std::shared_ptr<Entity> e);
	void addHighlight(std::shared_ptr<Entity> e);

	bool targetReached(const Vec2& pos, const Vec2& target) const;

	const std::string& levelPath() const { return m_level_path; }

	EntityManager& getEntityManager() { return m_entity_manager; }

	const PlayerConfig& getPlayerConfig() const { return m_player_config; }
	const std::vector<std::string> getLayerNames() const;

protected:
	Scene(GameEngine* engine);
	Scene(GameEngine* engine, const std::string& level_path);

	GameEngine* m_engine{ nullptr };
	EntityManager m_entity_manager;
	size_t m_current_frame{ 0 };
	std::map<int, ActionName> m_action_map;
	Zoom m_zoom;
	VertexArray m_hp_bars{ TRIANGLE };
	VertexArray m_highlights{ LINES };
	PlayerConfig m_player_config;

	bool m_paused{ false };
	bool m_has_ended{ false };
	bool m_draw_textures{ true };
	bool m_draw_collision{ false };
	bool m_draw_grid{ false };
	bool m_show_ai_info{ false };
	bool m_show_hp{ true };

	std::string m_level_path{ "" };

	Vec2 m_mouse_pos;
	Vec2 m_grid_size{ 0, 0 };
	Vec2 m_selected_cell{ 0, 0 };
	const Vec2 m_grid_cell_size{ 64, 64 };
	Text m_grid_text;

	std::vector<Layer> m_background_layers;

	virtual void onEnd() = 0;
	void setPaused(bool paused) { m_paused = paused; }

	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) const;
};

class SceneMenu: public Scene
{
	std::string m_title { "Menu" };
	std::vector<std::string> m_menu_strings;
	std::vector<std::string> m_level_paths;
	Text m_menu_text;
	size_t m_menu_index{ 0 }; // selected menu item
	unsigned int m_font_size{ 20 };
	ParticleSystem m_particles;

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
	std::shared_ptr<Entity> m_player;
	bool m_can_shoot{ true };
	bool m_can_jump{ true };

	sf::CircleShape m_mouse_shape;

public:
	SceneSideScroller(GameEngine* engine, const std::string& level_path);

	void init(const std::string& level_path);
	void update();

	void loadLevel(const std::string& path);

	void spawnPlayer();
	void spawnBullet();
	void spawnExplosion(const Vec2& pos);
	void spawnItem(const Vec2& pos, const std::string& animation_name, Tag tag);

	// Systems
	void sAI();
	void sAnimation();
	void sMovement();
	void sStatus();
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
	std::shared_ptr<Entity> m_player;
	bool m_follow{ false };
	bool m_can_attack{ true };
	Vec2 m_room_size{ 20, 12 };
	std::vector<Vec2> m_doorways;

public:
	SceneRPG(GameEngine* engine, const std::string& level_path);

	void init(const std::string& level_path);
	void update();

	void loadLevel(const std::string& path);

	void spawnPlayer();
	void spawnSword(std::shared_ptr<Entity> entity);
	void setSwordPos(std::shared_ptr<Entity> entity, const Vec2& facing, const Vec2& pos);
	void setRoomSize();
	void setFacing(std::shared_ptr<Entity> entity);

	void teleport(const Vec2& cur_doorway);

	Vec2 getPosition(float rx, float ry, float tx, float ty) const;
	Vec2 getPosition(float x, float y) const;
	Vec2 getCurrentRoom() const;

	void sAI();
	void sMovement();
	void sStatus();
	void sCollision();
	void sAnimation();
	void sCamera();
	void sDoAction(const Action& action);
	void sRender();
	void sDragAndDrop();
	void sInteract();

	void onEnd();
};
