#pragma once

#include "../math/vec2.hpp"

#include "../asset/animation.hpp"

enum class Tag : unsigned char {
	DEFAULT,
	TILE,
	PLAYER,
	ENEMY,
	BULLET,
	EXPLOSION,
	SWORD,
	HEART,
	TELEPORT,
	ELEVATOR,
	CHECKPOINT,
	TRIGGER,
	TRIGGERABLE
};

enum class State: unsigned char {
	NONE,
	JUMP,
	STAND,
	RUN,
	ATTACK,
};

// NOTE: just a suggestion
enum class Facing: unsigned char {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

enum class WeaponType: unsigned char {
	MELEE,
	RANGED
};

enum class TriggerType: unsigned char {
	NONE,
	APPLY_GRAVITY,
	DESTROY,
	CHANGE_SCENE,
	PLAY_SOUND,
	PLAY_MUSIC,
	START_CUTSCENE
};

struct Component {
	bool has{ false };
};

struct CTransform: Component {
	Vec2 pos{ 0.0f, 0.0f }; // Center of an object
	Vec2 velocity{ 0.0f, 0.0f };
	Vec2 prev_pos{ 0.0f, 0.0f };
	Vec2 scale{ 1.0f, 1.0f };
	Vec2 facing{ 1.0f, 0.0f };
	Vec2 prev_facing{ 1.0f, 0.0f };

	float angle{ 0.0f };

	// Check whether it's possible to apply transformation such as scale or rotation individually to an entity.
	// Explanation: we want to add as many entities' vertex data into one single vertex array to reduce draw() calls.
	bool transformable{ false };

	CTransform() {}
	CTransform(const Vec2& p, bool t = false) 
		: pos(p), transformable(t)
	{
	}

	CTransform(const Vec2& p, const Vec2& v, bool t = false) 
		: pos(p), velocity(v), transformable(t)
	{
	}

	CTransform(const Vec2& p, const Vec2& v, float a)
		: pos(p), velocity(v), angle(a), transformable(true)
	{
	}
};

struct CScore: Component {
	int score;

	CScore() {}
	CScore(int s)
		: score(s)
	{
	}
};

struct CHealth: Component {
	int max{ 4 };
	int current{ 4 };
	float percentage{ 1.0f }; // Store for rendering purposes

	CHealth() {}
	CHealth(int max_in)
		: max(max_in), current(max_in)
	{
	} 

	CHealth(int max_in, int current_in)
		: max(max_in),
		current(current_in),
		percentage(static_cast<float>(current_in)/static_cast<float>(max_in))
	{
	} 
};

struct CInput: Component {
	bool up{ false };
	bool left{ false };
	bool right{ false };
	bool down{ false };
	bool attack{ false };

	CInput() {}
};

struct CLifespan: Component {
	int remaining{ 0 }; // TODO: change to life_span
	int total{ 0 }; // TODO: change to frame_created

	CLifespan() {}
	CLifespan(int total)
		: remaining(total), total(total)
	{
	}
};

struct CDamage: Component {
	int damage{ 1 };

	CDamage() {}
	CDamage(int damage_in)
		: damage(damage_in)
	{
	}
};

struct CInvincibility: Component {
	int i_frames{ 60 };

	CInvincibility() {}
	CInvincibility(int i_frames_in)
		: i_frames(i_frames_in)
	{
	}
};

struct CBBox: Component {
	Vec2 size{ 0.0f, 0.0f };
	Vec2 half_size{ 0.0f, 0.0f }; // To reduce computations
	bool block_movement{ false };
	bool block_vision{ false };

	CBBox() {}
	CBBox(Vec2 size_in)
		: size(size_in), half_size(size_in/2)
	{
	}

	CBBox(Vec2 size_in, bool block_movement_in, bool block_vision_in)
		: size(size_in),
		half_size(size_in/2),
		block_movement(block_movement_in),
		block_vision(block_vision_in)
	{
	}
};

struct CAnimation: Component {
	Animation animation;
	bool repeat{ false };

	CAnimation() {}
	CAnimation(const Animation& animation_in)
		: animation(animation_in)
	{
	}

	CAnimation(const Animation& animation_in, bool repeat_in)
		: animation(animation_in), repeat(repeat_in)
	{
	}
};

struct CGravity: Component {
	float gravity{ 0 };

	CGravity() {}
	CGravity(float gravity_in)
		: gravity(gravity_in)
	{
	}
};

struct CState: Component {
	State state{ State::NONE };
	State prev_state{ State::NONE };

	// TODO: Add direction variable here
	//       which can be used for animations (facing Vec2 also be used)
	//       Maybe more complex states such as ATTACK_UP etc. could be removed.
	//       This could in the end solve sword logic/animation bugs

	CState() {}
	CState(State state_in)
		: state(state_in)
	{
	}
};

struct CDraggable: Component {
	bool dragged{ false };

	CDraggable() {}
};

struct CFollowPlayer: Component {
	Vec2 home{ 0, 0 };
	float speed{ 0.0f };
	bool detected{ false };

	CFollowPlayer() {}
	CFollowPlayer(Vec2 home_in, float velocity_in)
		: home(home_in), speed(velocity_in) 
	{
	}
};

struct CPatrol: Component {
	std::vector<Vec2> positions;
	size_t cur_pos{ 0 };
	float speed{ 0.0f };

	CPatrol() {}
	CPatrol(const std::vector<Vec2>& positions_in, float speed_in)
		: positions(positions_in), speed(speed_in)
	{
	}
};

struct CBehavior: Component {
	bool hostile{ false };

	CBehavior() {}
	CBehavior(bool hostile_in)
		: hostile(hostile_in)
	{
	}
};

struct CWeapon: Component {
	WeaponType type{ WeaponType::MELEE };
	int max_cooldown{ 20 };
	int remaining_cooldown{ 0 };
	size_t id{ 0 };

	CWeapon() {}
	CWeapon(WeaponType type_in) 
		: type(type_in)
	{
	}

	CWeapon(WeaponType type_in, int cooldown_in) 
		: type(type_in), max_cooldown(cooldown_in)
	{
	}

	CWeapon(WeaponType type_in, int cooldown_in, size_t id_in) 
		: type(type_in), max_cooldown(cooldown_in), id(id_in)
	{
	}
};

struct CPath: Component {
	std::vector<Vec2> positions;
	size_t cur_pos{ 0 };
	bool finished{ false };

	CPath() {}
	CPath(const std::vector<Vec2> positions_in)
		: positions(positions_in)
	{
	}
};

struct CTrigger: Component {
	// Scene change, start music, cutscene, etc.
	int id{ -1 };
	TriggerType type{ TriggerType::NONE };

	// TODO: Triggers could be activated based on timers etc.
	//       i.e. collision doesn't have to be the only way to activate

	CTrigger() {}
	CTrigger(int id_in, TriggerType type_in)
		: id(id_in), type(type_in)
	{
	}
};

struct CTriggerable: Component {
	int trigger_id{ -1 };

	CTriggerable() {}
	CTriggerable(int trigger_id_in)
		: trigger_id(trigger_id_in)
	{
	}
};

struct CInteractable: Component {
	// Interact with e.g. button, door, npc, item, etc.
	bool highlight{ false };
	bool active{ false };
};

struct CBreakable: Component {
	bool active{ true };
};

struct CMovable: Component {
	bool active{ true };
};

// NOTE: Combine this with CInteractable ^?
struct CInfo: Component {
	std::string name{ "" };
	std::string description{ "" };

	CInfo() {}
	CInfo(const std::string& name_in, const std::string& description_in)
		: name(name_in), description(description_in)
	{
	}
};

struct CDialog: Component {
	// Dialog tree for entities	
};

struct CLightSource: Component {
	// For dynamic lighting
};

struct CSound: Component {
	// Play different sounds related to certain entity
};

struct CGridCoordinate: Component {
	// NOTE: this could be used to check collision
	// For instance, against player p
	// [1][2][3]
	// [4][p][5]
	// [6][7][8]
	// We would only check collisions with the tiles in positions 1..8 and p,
	// if they have bbox
};
