// Header
#define _USE_MATH_DEFINES
#include <cmath>

#include "enemy.hpp"
#include "walls.hpp"
#include "barrel.hpp"

#include <string>

Texture Enemy::enemy_texture;
bool Enemy::change_texture;

bool Enemy::init(int current_level)
{
	std::string dir_path = data_path "/textures/";
	std::string enemy_path = dir_path + std::to_string(current_level) + "/" + "enemy.png";
	const char *texturePath = enemy_path.c_str();

	if (!enemy_texture.is_valid() || change_texture)
	{
		enemy_texture.destroy();
		if (!enemy_texture.load_from_file(texturePath))
		{
			fprintf(stderr, "Failed to load enemy texture!");
			return false;
		}
	}

	float spritesheet_width = 1.0f;
	float spritesheet_height = 1.0f;

	if (current_level == 0) {
		spritesheet_width = 8.0f;
	}
	else if (current_level == 1) {
		spritesheet_width = 4.0f;
	}
	else if (current_level == 2) {
		spritesheet_width = 6.0f;
	}
	else if (current_level == 3) {
		spritesheet_width = 4.0f;
	}
	else if (current_level == 4) {
		spritesheet_width = 4.0f;
	}
	else if (current_level == 5) {
		spritesheet_width = 2.0f;
	}
	else if (current_level == 6) {
		spritesheet_width = 6.0f;
	}
	else if (current_level == 7) {
		spritesheet_width = 8.0f;
	}


	m_sprite_width = enemy_texture.width / spritesheet_width;
	m_sprite_height = enemy_texture.height / spritesheet_height;

	spritesheet.init(&enemy_texture, { spritesheet_width, spritesheet_height }, this);
	spritesheet.set_data(this, 0);

	/*
	if (!RenderManager::set_buffers(&enemy_texture, this, -0.03f, vec2{1, 1}))
	{
		return false;
	}*/

	change_texture = false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	motion.radians = 0.f;
	motion.speed = 200.f;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture.
	physics.scale = {-0.8f, 0.8f};

	detect_distance = 400.f;
	attack_distance = 300.f;
	guard_distance = 120.f;
	state = rest;
	life = 1;
	m_current_level = current_level;
	return true;
}

// Releases all graphics resources
void Enemy::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Enemy::update(float ms, vec2 prince_location, std::vector<Walls> walls, std::vector<Barrel> barrels)
{
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	this->prince_location = prince_location;
	float distance_to_prince = len(sub(prince_location, motion.position));
	float distance_to_guard_pos = len(sub(guard_location, motion.position));

	if (distance_to_prince > detect_distance)
	{
		if (distance_to_guard_pos > guard_distance)
			state = return_guard_pos;
		else
			state = rest;
	}
	else if (distance_to_prince < detect_distance && distance_to_prince > attack_distance)
	{
		if (!is_wall_between_enemy_prince(prince_location, walls) && !is_barrel_between_enemy_prince(prince_location, barrels))
			state = chase_prince;
		else
			state = rest;
	}
	else if (distance_to_prince < attack_distance)
	{
		if (!is_wall_between_enemy_prince(prince_location, walls)&& !is_barrel_between_enemy_prince(prince_location, barrels))
			state = attack;
		else
			state = rest;
	}

	float step = 0.5 * motion.speed * (ms / 1000);
	if (state == chase_prince)
		move(mul(normalize(sub(prince_location,motion.position)), step));
	else if (state == return_guard_pos)
		move(mul(normalize(sub(guard_location, motion.position)), step));
}

void Enemy::draw(const mat3 &projection)
{
	animate();
	RenderManager::draw_texture(projection, &enemy_texture, this, motion.position, motion.radians, physics.scale);
}

void Enemy::animate()
{
	int total_frames = 8;
	int frame_index = 0;
	float animation_speed = 0.1f;

	m_animation_time += animation_speed;

	frame_index = frame_index + (int)m_animation_time % total_frames;

	spritesheet.update_data(this, frame_index);
}

bool Enemy::get_stop_and_attack()
{
	return state == attack;
}
vec2 Enemy::get_position() const
{
	return motion.position;
}

void Enemy::set_position(vec2 position)
{
	motion.position = position;
}

void Enemy::set_guard_position(vec2 position)
{
	guard_location = position;
}
vec2 Enemy::get_guard_position()
{
	return guard_location;
}
vec2 Enemy::get_bounding_box() const
{
	// Returns the local bounding coordinates scaled by the current size of the turtle
	// fabs is to avoid negative scale due to the facing direction.
	return { std::fabs(physics.scale.x) * m_sprite_width, std::fabs(physics.scale.y) * m_sprite_height };
}

bool Enemy::is_wall_between_enemy_prince(vec2 prince_location, std::vector<Walls> walls)
{
	float slope = (prince_location.y - motion.position.y) / (prince_location.x - motion.position.x);
	float intercept = prince_location.y - slope * prince_location.x;
	for (auto wall : walls)
	{
		vec2 wall_bottom_right = add(wall.get_position(), mul(wall.get_bounding_box(),0.5));
		vec2 wall_top_left = sub(wall.get_position(), mul(wall.get_bounding_box(),0.5));

		float y_left = slope * wall_top_left.x + intercept;
		float y_right = slope * wall_bottom_right.x + intercept;
		float x_top = (wall_top_left.y - intercept) / slope;
		float x_bottom = (wall_bottom_right.y - intercept) / slope;

		bool is_left = y_left < wall_bottom_right.y && y_left > wall_top_left.y &&
			is_in_between(wall_top_left.x, prince_location.x, motion.position.x) &&
			is_in_between(y_left, prince_location.y, motion.position.y);

		bool is_right = y_right < wall_bottom_right.y && y_right > wall_top_left.y &&
			is_in_between(wall_bottom_right.x, prince_location.x, motion.position.x) &&
			is_in_between(y_right, prince_location.y, motion.position.y);

		bool is_top = x_top > wall_top_left.x && x_top < wall_bottom_right.x &&
			is_in_between(wall_top_left.y, prince_location.y, motion.position.y) &&
			is_in_between(x_top, prince_location.x, motion.position.x);

		bool is_bottom = x_bottom > wall_top_left.x && x_bottom < wall_bottom_right.x &&
			is_in_between(wall_bottom_right.y, prince_location.y, motion.position.y) &&
			is_in_between(x_bottom, prince_location.x, motion.position.x);

		if (is_left || is_right || is_bottom || is_top)
			return true;
	}
	return false;
}
bool Enemy::is_barrel_between_enemy_prince(vec2 prince_location, std::vector<Barrel> barrels)
{
    float slope = (prince_location.y - motion.position.y) / (prince_location.x - motion.position.x);
    float intercept = prince_location.y - slope * prince_location.x;
    for (auto barrel : barrels)
    {
        vec2 barrels_bottom_right = add(barrel.get_position(), mul(barrel.get_bounding_box(), 0.5));
        vec2 barrels_top_left = sub(barrel.get_position(), mul(barrel.get_bounding_box(), 0.5));

        float y_left = slope * barrels_top_left.x + intercept;
        float y_right = slope * barrels_bottom_right.x + intercept;
        float x_top = (barrels_top_left.y - intercept) / slope;
        float x_bottom = (barrels_bottom_right.y - intercept) / slope;

        bool is_left = y_left < barrels_bottom_right.y && y_left > barrels_top_left.y &&
                       is_in_between(barrels_top_left.x, prince_location.x, motion.position.x) &&
                       is_in_between(y_left, prince_location.y, motion.position.y);

        bool is_right = y_right < barrels_bottom_right.y && y_right > barrels_top_left.y &&
                        is_in_between(barrels_bottom_right.x, prince_location.x, motion.position.x) &&
                        is_in_between(y_right, prince_location.y, motion.position.y);

        bool is_top = x_top > barrels_top_left.x && x_top < barrels_bottom_right.x &&
                      is_in_between(barrels_top_left.y, prince_location.y, motion.position.y) &&
                      is_in_between(x_top, prince_location.x, motion.position.x);

        bool is_bottom = x_bottom > barrels_top_left.x && x_bottom < barrels_bottom_right.x &&
                         is_in_between(barrels_bottom_right.y, prince_location.y, motion.position.y) &&
                         is_in_between(x_bottom, prince_location.x, motion.position.x);

        if (is_left || is_right || is_bottom || is_top)
            return true;
    }
    return false;
}
void Enemy::set_texture_change()
{
	change_texture = true;
}

bool Enemy::collides_with(const Walls &wall, float ms)
{
	vec2 enemy_bounding_box = get_bounding_box();
	vec2 wall_bounding_box = wall.get_bounding_box();
	vec2 wall_pos = wall.get_position();

	float step = -0.5 * motion.speed * ms / 1000;
	vec2 enemy_next_pos = motion.position;
	if (state == chase_prince)
	{
		enemy_next_pos = add(motion.position, mul(sub(motion.position, prince_location), step / len(sub(prince_location, motion.position))));
	}
	else if (state == return_guard_pos)
	{
		enemy_next_pos = add(motion.position, mul(sub(motion.position, guard_location), step * 3 / len(sub(guard_location, motion.position))));
	}

	float enemy_off_set_x = enemy_bounding_box.x / 2;
	float enemy_off_set_y = enemy_bounding_box.y / 2;

	float enemy_right = enemy_next_pos.x + enemy_off_set_x;
	float enemy_left = enemy_next_pos.x - enemy_off_set_x;
	float enemy_top = enemy_next_pos.y - enemy_off_set_y;
	float enemy_bottom = enemy_next_pos.y + enemy_off_set_y;

	float wall_off_set_x = wall_bounding_box.x / 2;
	float wall_off_set_y = wall_bounding_box.y / 2;

	float wall_right = wall_pos.x + wall_off_set_x;
	float wall_left = wall_pos.x - wall_off_set_x;
	float wall_top = wall_pos.y - wall_off_set_y;
	float wall_bottom = wall_pos.y + wall_off_set_y;

	if (enemy_top <= wall_bottom && enemy_bottom >= wall_top && enemy_left <= wall_right && enemy_right >= wall_left)
		return true;
	else
		return false;
}
bool Enemy::collides_with(const Barrel &barrel, float ms)
{
    vec2 enemy_bounding_box = get_bounding_box();
    vec2 wall_bounding_box = barrel.get_bounding_box();
    vec2 wall_pos = barrel.get_position();

    float step = -0.5 * motion.speed * ms / 1000;
    vec2 enemy_next_pos = motion.position;
    if (state == chase_prince)
    {
        enemy_next_pos = add(motion.position, mul(sub(motion.position, prince_location), step / len(sub(prince_location, motion.position))));
    }
    else if (state == return_guard_pos)
    {
        enemy_next_pos = add(motion.position, mul(sub(motion.position, guard_location), step * 3 / len(sub(guard_location, motion.position))));
    }

    float enemy_off_set_x = enemy_bounding_box.x / 2;
    float enemy_off_set_y = enemy_bounding_box.y / 2;

    float enemy_right = enemy_next_pos.x + enemy_off_set_x;
    float enemy_left = enemy_next_pos.x - enemy_off_set_x;
    float enemy_top = enemy_next_pos.y - enemy_off_set_y;
    float enemy_bottom = enemy_next_pos.y + enemy_off_set_y;

    float wall_off_set_x = wall_bounding_box.x / 2;
    float wall_off_set_y = wall_bounding_box.y / 2;

    float wall_right = wall_pos.x + wall_off_set_x;
    float wall_left = wall_pos.x - wall_off_set_x;
    float wall_top = wall_pos.y - wall_off_set_y;
    float wall_bottom = wall_pos.y + wall_off_set_y;

    if (enemy_top <= wall_bottom && enemy_bottom >= wall_top && enemy_left <= wall_right && enemy_right >= wall_left)
        return true;
    else
        return false;
}
void Enemy::move(vec2 off)
{
	motion.position = add(motion.position, off);
}

void Enemy::set_key(int key)
{
	this->key = key;
}

float Enemy::get_angle()
{
	float angle = atan2(prince_location.y - motion.position.y, prince_location.x - motion.position.x) + M_PI / 2;
	return angle;
}

void Enemy::minus_life()
{
	life--;
}

bool Enemy::enemy_kill()
{
	return life <= 0;
}
