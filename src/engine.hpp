#pragma once

#include "ecs/entity-manager.hpp"
#include "asset/assets.hpp"
#include "editor/editor.hpp"
#include "math/vec2.hpp"
#include "core/render-window.hpp"

struct PlayerConfig;

class Scene;

enum class SceneType: unsigned char;

class GameEngine {
	std::string m_title{ "GardenTool2D" };
	std::string m_cur_song_name{ "" };
	RenderWindow m_window;
	bool m_paused{ false };
	bool m_running{ true };
	bool m_edit_mode{ false };
	bool m_enable_sound{ true };
	bool m_enable_music{ true };
	bool m_vsync{ true };
	unsigned int m_screen_w{ 1280 };
	unsigned int m_screen_h{ 768 };
	unsigned int m_framerate{ 60 };

	std::map<SceneType, std::shared_ptr<Scene>> m_scenes;
	SceneType m_cur_scene;

	Assets m_assets;
	Editor m_editor;

	std::vector<size_t> m_selected_entity_ids;
	std::vector<Vec2> m_selected_pos{ {0, 0} };

	void init(const std::string& config);
	void setPaused(bool paused);
	void update();

	void sUserInput();

	std::shared_ptr<Scene> currentScene() { return m_scenes[m_cur_scene]; }

public:
	GameEngine(const std::string& config);

	void run();
	void quit();

	void changeScene(const SceneType scene_name, std::shared_ptr<Scene> scene);
	void playSound(const std::string& sound_name);
	void playMusic(const std::string& music_name);
	void stopMusic(const std::string& music_name);

	void toggleSound() { m_enable_sound = !m_enable_sound; }
	void toggleMusic() { m_enable_music = !m_enable_music; }
	void toggleEditMode();
	void pushSelectedPos(const Vec2& cell, bool reset = false);
	void pushSelectedEntityId(size_t id, bool reset = false);
	void popSelectedPos(const Vec2& cell);
	void popSelectedEntityId(size_t id);
	void cleanEditMode();

	RenderWindow& window() { return m_window; };
	const Assets& assets() const { return m_assets; };
	size_t selectedEntityId() const { return m_selected_entity_ids.back(); }
	const Vec2& selectedPos() const { return m_selected_pos.back(); }
	const std::vector<size_t>& allSelectedEntityIds() const { return m_selected_entity_ids; }
	const std::vector<Vec2>& allSelectedPos() const { return m_selected_pos; }
	const Vec2 toGridPos(const Vec2& pos, const Vec2& size);
	const PlayerConfig& getPlayerConfig();
	bool editMode() const { return m_edit_mode; };
	bool isRunning();
	const std::string& currentLevelPath();
	const std::vector<std::string> layerNames();
};
