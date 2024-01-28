#pragma once

#include "EntityManager.h"
#include "Assets.h"

class Scene;

enum class SceneType: unsigned char;

class GameEngine
{
	std::string m_title{ "2D Game" };
	sf::RenderWindow m_window;
	bool m_paused{ false };
	bool m_running{ true };
	unsigned int m_screen_w{ 1280 };
	unsigned int m_screen_h{ 768 };
	unsigned int m_framerate{ 60 };

	std::map<SceneType, std::shared_ptr<Scene>> m_scenes;
	SceneType m_cur_scene;

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

	void changeScene(const SceneType scene_name, std::shared_ptr<Scene> scene, bool end_current_scene = false);
	void playSound(const std::string& sound_name);
	void playMusic(const std::string& music_name);
	void stopMusic(const std::string& music_name);

	sf::RenderWindow& window() { return m_window; };
	const Assets& assets() const { return m_assets; };
	bool isRunning();
};
