//
// Created by Yichun Liu on 2019-12-07.
//
#define _USE_MATH_DEFINES

#include <cmath>
#include <string>
#include "barrel.hpp"

Texture Barrel::barrel_texture;
bool Barrel::change_textures;

bool Barrel::init(int current_level)
{
    std::string dir_path = data_path "/textures/";
    std::string basic_path = dir_path + std::to_string(current_level) + "/";

    std::string barrel_path = basic_path + "barrel.png";
    const char *barrel_path_char = barrel_path.c_str();

    if (!RenderManager::load_texture(barrel_path_char, &barrel_texture, this))
    {
        return false;
    }

    if (!RenderManager::set_buffers(&barrel_texture, this, -0.01, vec2{2, 1}))
    {
        return false;
    }

    // Loading shaders
    if (!effect.load_from_file(shader_path("num.vs.glsl"), shader_path("num.fs.glsl")))
        return false;

    motion.radians = M_PI;
    motion.speed = 0.0f;

    // Setting initial values, scale is negative to make it face the opposite way
    // 1.0 would be as big as the original texture.
    physics.scale = {0.6f, 1.2f};
    m_particles.init();
    is_broken = false;
    return true;
}

void Barrel::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);

	m_particles.destroy();
}

void Barrel::update(float ms)
{
    m_particles.update(ms);
}

void Barrel::draw(const mat3 &projection)
{
    glEnable(GL_DEPTH_TEST);
    Entity *entity = this;
    Texture *texture = &barrel_texture;

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
    float ox = is_broken / 2.0;
    float oy = 0.f;
    float offset_in[2] = {ox, oy};
    glUniform3fv(color_uloc, 1, color);
    glUniform2fv(offset_uloc, 1, offset_in);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    glDisable(GL_DEPTH_TEST);
    m_particles.draw(projection);
}

vec2 Barrel::get_position() const
{
    return motion.position;
}

void Barrel::set_position(vec2 position)
{
    motion.position = position;
}

vec2 Barrel::get_bounding_box() const
{
    // Returns the local bounding coordinates scaled by the current size of the star
    // fabs is to avoid negative scale due to the facing direction.
    return {std::fabs(physics.scale.x) * (barrel_texture.width - 100),
            std::fabs(physics.scale.y) * barrel_texture.height};
}

void Barrel::set_texture_change()
{
    change_textures = true;
}

void Barrel::unset_texture_change()
{
    change_textures = false;
}
void Barrel::clear_pebbles(){
    m_particles.kill();
};
int Barrel::hit_by_bullet()
{
    if(is_broken){
        return 0;
    }else {
        is_broken = true;
        m_particles.spawn_pebble(motion.position, 5);
        return rand() % 3;
    }
}

