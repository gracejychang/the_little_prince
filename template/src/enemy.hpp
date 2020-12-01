#pragma once

#include "common.hpp"
#include "render_manager.hpp"
#include "bullet.hpp"
#include "barrel.hpp"
#include "spritesheet.hpp"

#include <vector>

// Salmon enemy
class Walls;

class Enemy : public Entity
{
	// Shared between all turtles, no need to load one for each instance
	static Texture enemy_texture;
	static bool change_texture;
	//the minimal distance between prince and enemy
	float detect_distance;
	float guard_distance;
	float attack_distance;
	vec2 prince_location;
	vec2 guard_location;
	int key;
	int life;
	enum state
	{
		rest,
		chase_prince,
		attack,
		return_guard_pos
	} state;
	float weight = 200.f;

public:
	SpriteSheet spritesheet;

	// Creates all the associated render resources and default transform
	bool init(int current_level);

	// Releases all the associated resources
	void destroy();

	// Update turtle due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms, vec2 prince_location, std::vector<Walls> walls, std::vector<Barrel> barrels);

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3 &projection) override;

	// Returns the current turtle position
	vec2 get_position() const;

	// Sets the new turtle position
	void set_position(vec2 position);

	// set the guard location  of the enemy
	void set_guard_position(vec2 position);
	vec2 get_guard_position();

	// Returns the turtle' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box() const;

	static void set_texture_change();
	void animate();

	bool collides_with(const Walls &wall, float ms);
	bool collides_with(const Barrel &barrel, float ms);
    bool is_wall_between_enemy_prince(vec2 prince_location, std::vector<Walls> walls);
	bool is_barrel_between_enemy_prince(vec2 prince_location, std::vector<Barrel> barrel);
    void move(vec2 off);
	void set_key(int key);

	int get_key() { return key; }

	bool get_stop_and_attack();

	float get_angle();

	void minus_life();

	bool enemy_kill();

	//set origin location and when enemy is far away from prince then it will go back to it location;
private:
	float m_animation_time = 0.f;

	float m_sprite_width;
	float m_sprite_height;

	int m_current_level;
};
