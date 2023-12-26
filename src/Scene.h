#pragma once

#include <map>
#include <memory>
#include <string>
#include "EntityManager.h"
#include "Action.h"

class GameEngine;

class Scene
{
protected:
	std::shared_ptr<GameEngine> m_game_engine;
	EntityManager m_entities;
	int current_frame{ 0 };
	std::map<int, std::string> m_action_map;
	bool paused{ false };
	bool hasEnded{ false };

	virtual void update() = 0;
	virtual void sDoAction(Action action) = 0; // (Action action as param)
	virtual void sRender() = 0;

	virtual void simulate();
	virtual void doAction(Action action); // (Action action as param)
	virtual void registerAction();
};


class ScenePlay: public Scene {
	std::string m_level_path{""};
	std::shared_ptr<Entity> m_player;
	// playerConfig

public:
	void init();
	void update();

	// Systems
	void sAnimation();
	void sMovement();
	void sEnemySpawner();
	void sCollision();
	void sRender();
	void sDoAction(Action action);
	void sDebug();
};

class SceneMenu: public Scene {

};
