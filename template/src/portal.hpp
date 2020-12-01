#pragma once

#include "common.hpp"
#include <vector>
#include "render_manager.hpp"


class Portal : public Entity
{
    // Shared between all fish, no need to load one for each instance

public:
    // Creates all the associated render resources and default transform
    bool init();

    // Releases all the associated resources
    void destroy();

    // Update fish
    // ms represents the number of milliseconds elapsed from the previous update() call
    void update();

    // Renders the fish
    // projection is the 2D orthographic projection matrix
    void draw(const mat3 &projection) override;

    // Returns the current fish position
    vec2 get_position() const;

    // Sets the new fish position
    void set_position(vec2 position);

    // Returns the fish' bounding box for collision detection, called by collides_with()
    vec2 get_bounding_box() const;
    bool is_in_boundaries(float top, float bottom, float left, float right);

private:
    bool is_verticle;
    std::vector<Vertex> m_vertices;  // all the point of the triangle
    std::vector<uint16_t> m_indices; // all the triangles to make a box
};
