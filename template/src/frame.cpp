// Header
#include "frame.hpp"

#include <cmath>

Texture Frame::frame_texture;

bool Frame::init()
{
	float z = 0.f;
	static const GLfloat screen_vertex_buffer_data[] = {
		-1.05f, -1.05f, z,
		1.05f, -1.05f, z,
		-1.05f, 1.05f, z,
		-1.05f, 1.05f, z,
		1.05f, -1.05f, z,
		1.05f, 1.05f, z,
	};

	frame_texture.load_from_file(textures_path("quit.png"));
	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertex_buffer_data), screen_vertex_buffer_data, GL_STATIC_DRAW);

	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("background.vs.glsl"), shader_path("background.fs.glsl")))
		return false;

	return true;
}

// Releases all graphics resources
void Frame::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Frame::draw(const mat3& projection)
{

		// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Setting shaders
	glUseProgram(effect.program);

	// Set screen_texture sampling to texture unit 0
	// Set clock
	GLuint screen_text_uloc = glGetUniformLocation(effect.program, "screen_texture");
	glUniform1i(screen_text_uloc, 0);

	// Draw the screen texture on the quad geometry
	// Setting vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frame_texture.id);

	// Bind to attribute 0 (in_position) as in the vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	// Draw
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	glDisableVertexAttribArray(0);
}


vec2 Frame::get_position()const
{
	return motion.position;
}

void Frame::set_position(vec2 position)
{
	motion.position = position;
}

GLuint Frame::get_texture_id()
{
	return frame_texture.id;
}
