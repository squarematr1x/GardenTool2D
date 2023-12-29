#pragma once

#include <map>
#include <memory>
#include <string>
#include "EntityManager.h"
#include "Action.h"

class GameEngine;

class Scene
{
public:
	Scene(GameEngine* game_engine)
		: m_game_engine(game_engine) {}

	virtual void update() = 0;
	virtual void sDoAction(const Action& action) = 0; // (Action action as param)
	virtual void sRender() = 0;

	virtual void simulate();
	virtual void doAction(const Action& action); // (Action action as param)
	virtual void registerAction(int key, const std::string& action_name);

	size_t width() const;
	size_t height() const;
	size_t currentFrame() const;

	bool hasEnded() const { return m_has_ended; }
	const std::map<int, std::string>& getActionMap() const { return m_action_map; }
	void drawLine(const Vec2& p1, const Vec2& p2);

protected:
	GameEngine* m_game_engine;
	EntityManager m_entity_manager;
	int current_frame{ 0 }; // TODO: use size_t type here?
	std::map<int, std::string> m_action_map; // SFML key -> action name
	bool m_paused{ false };
	bool m_has_ended{ false };

	virtual void onEnd() = 0;
	void setPaused(bool paused) { m_paused = paused; }
};


class ScenePlay: public Scene {
	struct PlayerConfig {
		float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};
	std::string m_level_path{""};
	std::shared_ptr<Entity> m_player;
	PlayerConfig m_player_config;
	bool m_draw_textures{false};
	bool m_draw_collision{false};
	bool m_draw_grid{false};
	const Vec2 m_grid_size{64, 64};
	sf::Text m_grid_text;

public:
	ScenePlay::ScenePlay(GameEngine* game_engine, const std::string& level_path);
	void init(const std::string& level_path);
	void update();

	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

	void loadLevel(const std::string& path);
	void registerAction(int key, const std::string& action_name);

	// Systems
	void sAnimation();
	void sMovement();
	void sEnemySpawner();
	void sCollision();
	void sRender();
	void sDoAction(const Action& action);
	void sDebug();
};

class SceneMenu: public Scene {
protected:
	std::string m_title;
	std::vector<std::string> m_menu_strings;
	std::vector<std::string> m_level_paths;
	sf::Text m_menu_text;
	size_t m_selected_menu_index{ 0 };

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);

public:
	SceneMenu(GameEngine* game_engine = nullptr);
	void registerAction(int key, const std::string& action_name);
	void sRender();
};
