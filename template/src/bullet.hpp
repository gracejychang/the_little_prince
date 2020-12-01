#pragma once
#pragma once

#include "common.hpp"
#include "render_manager.hpp"
// #include "game_object.hpp"

class Bullet : public Entity
{
	// Shared between all fish, no need to load one for each instance
	static Texture bullet_texture;

public:
	bool init(int index = 0);

	void destroy();

	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the fish
	// projection is the 2D orthographic projection matrix
	void draw(const mat3 &projection) override;

	vec2 get_position() const;

	void set_position(vec2 position);

	// Returns the fish' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box() const;

	// Moves the star's position by the specified offset
	void move(vec2 off);
	void set_rotation(float radians);
    void set_state(int w_a_h);
    int get_state();
    bool is_ready();
	float get_radians() { return motion.radians; }
	//the number is the index of different bullet in the texture;
	void set_bullet(int n);

private:
    enum state
    {
        waiting_for_next_attack,
        attacking,
        hitted
    } state;
	int counting_down;
	float offset[2];
};
