#pragma once

#include "EntityManager.h"
#include "Assets.h"

class Scene;

class GameEngine
{
	std::string m_title{ "2D Game" };
	sf::RenderWindow m_window;
	bool m_paused{ false };
	bool m_running{ true };
	unsigned int m_screen_w{ 1280 };
	unsigned int m_screen_h{ 768 };
	unsigned int m_framerate{ 60 };

	std::map<std::string, std::shared_ptr<Scene>> m_scenes;
	std::string m_cur_scene;

	Assets m_assets;

	void init(const std::string& config);
	void setPaused(bool paused);
	void update();

	void sUserInput();

	std::shared_ptr<Scene> currentScene() { return m_scenes[m_cur_scene]; }

public:
	GameEngine(const std::string& config);

	void run();
	void quit();

	void changeScene(const std::string& scene_name, std::shared_ptr<Scene> scene, bool end_current_scene = false);

	sf::RenderWindow& window() { return m_window; };
	const Assets& assets() const { return m_assets; };
	bool isRunning();
};
