//
// Created by Yichun Liu on 2019-12-07.
//

#pragma once

#include "common.hpp"
#include "render_manager.hpp"
#include "ps.hpp"


class Barrel : public Entity{
    static Texture barrel_texture;

public:
    static bool change_textures;
    // Creates all the associated render resources and default transform
    bool init(int current_level);

    // Releases all the associated resources
    void destroy();

    // Update fish
    // ms represents the number of milliseconds elapsed from the previous update() call
    void update(float ms);

    // Renders the fish
    // projection is the 2D orthographic projection matrix
    void draw(const mat3 &projection) override;

    // Returns the current fish position
    vec2 get_position() const;

    // Sets the new fish position
    void set_position(vec2 position);
    void clear_pebbles();
    vec2 get_bounding_box() const;
    static void set_texture_change();
    static void unset_texture_change();
    int hit_by_bullet();
    bool get_is_broken(){return is_broken;}
private:
    Particles m_particles;
    bool is_broken;
};
