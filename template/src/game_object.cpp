#include "game_object.hpp"
//#include "collision_manager.hpp"

bool GameObject::init()
{
	return true;
}

void GameObject::destroy()
{
	//CollisionManager::get_instance().unregister_collision_game_object(this);
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

vec2 GameObject::get_position() const
{
	return motion.position;
}

void GameObject::set_position(vec2 position)
{
	motion.position = position;
}

void GameObject::set_rotation(float radians)
{
	motion.radians = radians;
}