// Header
#define _USE_MATH_DEFINES
#include <cmath>

#include "bullet.hpp"
#include "walls.hpp"
#include <string>
Texture Bullet::bullet_texture;

bool Bullet::init(int index)
{

	//std::string dir_path = data_path "/textures/";
	//std::string bullet_path = dir_path + "bullet/bullet" + std::to_string(index) + ".png";
	//const char *texturePath = bullet_path.c_str();
	///*if (!RenderManager::load_texture(texturePath, &bullet_texture, this))
	//{
	//	return false;
	//}*/
	//if (!bullet_texture.is_valid())
	//{
	//	if (!bullet_texture.load_from_file(texturePath))
	//	{
	//		fprintf(stderr, "Failed to load turtle texture!");
	//		return false;
	//	}
	//}

	//if (!RenderManager::set_buffers(&bullet_texture, this, -0.01, vec2{1, 1}))
	//{
	//	return false;
	////}
	//std::string dir_path = data_path "/textures/";
	//std::string bullet_path = dir_path + "bullet/bullet" + ".png";
	//const char *texturePath = bullet_path.c_str();
	//if (!RenderManager::load_texture(texturePath, &bullet_texture, this))
	//{
	//	return false;
	//}

	//float spritesheet_width = 8.0f;
	//float spritesheet_height = 1.0f;

	//m_sprite_width = bullet_texture.width / spritesheet_width;
	//m_sprite_height = bullet_texture.height / spritesheet_height;

	//spritesheet.init(&bullet_texture, { spritesheet_width, spritesheet_height }, this);
	//spritesheet.set_data(this, 8);

	//// Loading shaders
	//if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
	//	return false;
	const char *texturePath = textures_path("bullet/bullet.png");
	if (!RenderManager::load_texture(texturePath, &bullet_texture, this))
	{
		return false;
	}

	if (!RenderManager::set_buffers(&bullet_texture, this, -0.01, vec2{1, 8}))
	{
		return false;
	}

	// Loading shaders
	if (!effect.load_from_file(shader_path("bullet.vs.glsl"), shader_path("bullet.fs.glsl")))
		return false;
	motion.radians = 0.f;
	motion.speed = 400.f;
	physics.scale = {0.62f, -0.08f};
	offset[0] = 0.0;
	if (index >= 0 && index < 8)
	{
		if (index == 0)
		{
			index = 1;
		}
		offset[1] = index;
	}
	else
		offset[1] = 8.0;
	state = waiting_for_next_attack;
	counting_down = 0;
	return true;
}

void Bullet::update(float m)
{
	float step = 1.5f * motion.speed * (m / 1000);
	//dy playable
	if (state == attacking)
	{
		float x = step * sin(motion.radians);
		float y = -step * cos(motion.radians);
		move({x, y});
	}
	else if (state == waiting_for_next_attack)
	{
		if (counting_down > 0)
			counting_down -= 1;
	}
	else if (state == hitted)
	{
		counting_down = 150;
		state = waiting_for_next_attack;
	}
}

void Bullet::draw(const mat3 &projection)
{
	/*RenderManager::draw_texture(projection, &bullet_texture, this, motion.position, motion.radians, physics.scale);*/
	glEnable(GL_DEPTH_TEST);
	Entity *entity = this;
	Texture *texture = &bullet_texture;

	transform.begin();
	transform.translate(motion.position);
	entity->transform.rotate(motion.radians);
	entity->transform.scale(physics.scale);
	entity->transform.end();
	// Setting shaders
	glUseProgram(entity->effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(entity->effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(entity->effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(entity->effect.program, "projection");
	GLint offset_uloc = glGetUniformLocation(entity->effect.program, "offset");

	// Setting vertices and indices
	glBindVertexArray(entity->mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, entity->mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity->mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(entity->effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(entity->effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&entity->transform.out);
	float color[] = {1.f, 1.f, 1.f};
	float ox = offset[0] / 1.0;
	float oy = offset[1] / 8.0;
	float offset_in[2] = {ox, oy};
	glUniform3fv(color_uloc, 1, color);
	glUniform2fv(offset_uloc, 1, offset_in);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	glDisable(GL_DEPTH_TEST);
}
bool Bullet::is_ready()
{
	return counting_down == 0;
}
void Bullet::set_bullet(int n)
{
	if (n > 0 && n < 9)
	{
		offset[1] = n;
	}
}
void Bullet::destroy()
{
	//CollisionManager::get_instance().unregister_collision_game_object(this);
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

vec2 Bullet::get_bounding_box() const
{
	// Returns the local bounding coordinates scaled by the current size of the star
	// fabs is to avoid negative scale due to the facing direction.
	return {std::fabs(physics.scale.x) * bullet_texture.width, std::fabs(physics.scale.y) * bullet_texture.height};
}

void Bullet::move(vec2 off)
{
	motion.position.x += off.x;
	motion.position.y += off.y;
}

void Bullet::set_rotation(float radians)
{
	motion.radians = radians;
}

vec2 Bullet::get_position() const
{
	return motion.position;
}

void Bullet::set_position(vec2 position)
{
	motion.position = position;
}

void Bullet::set_state(int w_a_h)
{
	if (w_a_h == 0)
		state = waiting_for_next_attack;
	else if (w_a_h == 1)
		state = attacking;
	else if (w_a_h == 2)
		state = hitted;
}
int Bullet::get_state()
{
	if (state == waiting_for_next_attack)
		return 0;
	else if (state == attacking)
		return 1;
	else if (state == hitted)
		return 2;
}
