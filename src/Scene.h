#pragma once

#include <map>
#include <memory>
#include <string>
#include "EntityManager.h"

class GameEngine;

class Scene
{
public:


protected:
	std::shared_ptr<GameEngine> m_game_engine;
	EntityManager m_entities;
	int current_frame{ 0 };
	std::map<int, std::string> m_action_map;
	bool paused{ false };
	bool hasEnded{ false };
};