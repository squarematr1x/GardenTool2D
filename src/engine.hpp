#pragma once

#include "ecs/entity-manager.hpp"
#include "assets.hpp"
#include "editor/editor.hpp"

class Scene;

enum class SceneType: unsigned char;

class GameEngine
{
	std::string m_title{ "GardenTool2D" };
	std::string m_cur_song_name{ "" };
	sf::RenderWindow m_window;
	bool m_paused{ false };
	bool m_running{ true };
	bool m_edit_mode{ false };
	bool m_enable_sound{ true };
	bool m_enable_music{ true };
	unsigned int m_screen_w{ 1280 };
	unsigned int m_screen_h{ 768 };
	unsigned int m_framerate{ 60 };

	std::map<SceneType, std::shared_ptr<Scene>> m_scenes;
	SceneType m_cur_scene;

	Assets m_assets;
	Editor m_editor;

	size_t m_selected_entity_id;

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

	void toggleSound() { m_enable_sound = !m_enable_sound; }
	void toggleMusic() { m_enable_music = !m_enable_music; }
	void toggleEditMode() { m_edit_mode = !m_edit_mode; }
	void setEditMode(bool edit) { m_edit_mode = edit; m_paused = edit; }
	void setSelectedEntityId(size_t id) { m_selected_entity_id = id; }

	sf::RenderWindow& window() { return m_window; };
	const Assets& assets() const { return m_assets; };
	size_t selectedEntityId() const { return m_selected_entity_id; }
	bool editMode() const { return m_edit_mode; };
	bool isRunning();
	const std::string& currentLevelPath();
};
