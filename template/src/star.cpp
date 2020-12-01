// Header
#define _USE_MATH_DEFINES
#include <cmath>

#include "star.hpp"

Texture Star::star_texture;

bool Star::init()
{
	const char *texturePath = textures_path("star.png");
	if (!RenderManager::load_texture(texturePath, &star_texture, this))
	{
		return false;
	}

	if (!RenderManager::set_buffers(&star_texture, this, -0.02f, vec2{1, 1}))
	{
		return false;
	}

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	motion.radians = 0.f;
	motion.speed = 400.f;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture.
	physics.scale = {-0.4f, 0.4f};
	fly_on = 0;
	state = 0; //dy playable

	return true;
}

void Star::update(float m)
{
	//dy playable

	float a = 20.f;
	if (state == 3)
	{
		motion.position = vec2{30, 60};
		motion.radians = motion.radians + a;
		physics.scale = vec2{0.4, 0.4};
	}
	if (fly_on != 0)
	{
		float detx = motion.position.x - 30;
		float dety = motion.position.y - 60;
		float r = sqrt(pow(dety, 2) + pow(detx, 2));
		float step = 0.5 * motion.speed;

		if (motion.position.x > 30)
		{
			motion.speed = motion.speed + a;
			physics.scale.x = physics.scale.x - 0.01;
			physics.scale.y = physics.scale.y - 0.01;
			state = 1;
			move(vec2{-detx * step / (r * 100), -dety * step / (r * 100)});
		}
		else
		{
			state = 2;
			physics.scale = vec2{0.4, 0.4};
		}
		motion.radians = motion.radians + a;
	}
}

void Star::draw(const mat3 &projection)
{
	RenderManager::draw_texture(projection, &star_texture, this, motion.position, motion.radians, physics.scale);
}

void Star::destroy()
{
	//CollisionManager::get_instance().unregister_collision_game_object(this);
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

vec2 Star::get_bounding_box() const
{
	// Returns the local bounding coordinates scaled by the current size of the star
	// fabs is to avoid negative scale due to the facing direction.
	vec2 offset = {40.f, 30.f};
	offset = sub({std::fabs(physics.scale.x) * star_texture.width, std::fabs(physics.scale.y) * star_texture.height}, offset);
	return offset;
}

void Star::move(vec2 off)
{
	motion.position.x += off.x;
	motion.position.y += off.y;
}

// not sure if getting star state is still needed?
int Star::get_star_state()
{
	return state;
}

vec2 Star::get_position() const
{
	return motion.position;
}

void Star::set_position(vec2 position)
{
	motion.position = position;
}

void Star::change_state(int s)
{
	state = s;
}
