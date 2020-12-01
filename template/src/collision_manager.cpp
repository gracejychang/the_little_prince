#define _USE_MATH_DEFINES
#include <cmath>

#include "collision_manager.hpp"
#include "game_object.hpp"
#include "prince.hpp"
#include "star.hpp"
#include "walls.hpp"
#include "heart.hpp"
#include "enemy.hpp"
#include "key.hpp"
#include "barrel.hpp"
#include "portal.hpp"
#include "common.hpp"
#include "bullet.hpp"

#include <string>
#include <algorithm>

// Simple bounding box collision check
// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You don't
// need to try to use this technique.
bool CollisionManager::prince_collides_with_enemy(Prince *prince, Enemy *enemy)
{
    float dx = prince->motion.position.x - enemy->get_position().x;
    float dy = prince->motion.position.y - enemy->get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(enemy->get_bounding_box().x, enemy->get_bounding_box().y);
    float my_r = std::max(prince->physics.scale.x, prince->physics.scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
    {
        return true;
    }
    return false;
}

bool CollisionManager::prince_collides_with_portal(Prince *prince, Portal &portal)
{
    vec2 bb_tl = sub(portal.get_position(), portal.get_bounding_box());
    vec2 bb_br = add(portal.get_position(), portal.get_bounding_box());
    bool is_in_bb = (portal.get_position().x > bb_tl.x) && (portal.get_position().y > bb_tl.y) &&
                    (portal.get_position().x < bb_br.x) && (portal.get_position().y < bb_br.y);
    if (is_in_bb)
    {
        vec2 top_left = sub(prince->get_position(), mul(prince->get_bounding_box(), 0.3));
        vec2 bot_right = add(prince->get_position(), mul(prince->get_bounding_box(), 0.3));
        return portal.is_in_boundaries(top_left.y, bot_right.y, top_left.x, bot_right.x);
    }
    return false;
}

bool CollisionManager::prince_collides_with_heart(Prince *prince, Heart &heart)
{
    float dx = prince->motion.position.x - heart.get_position().x;
    float dy = prince->motion.position.y - heart.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(heart.get_bounding_box().x, heart.get_bounding_box().y);
    float my_r = std::max(prince->physics.scale.x, prince->physics.scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.9f;
    if (d_sq < r * r)
        return true;
    return false;
}

bool CollisionManager::prince_collides_with_key(Prince *prince, Key &key)
{
    float dx = prince->motion.position.x - key.get_position().x;
    float dy = prince->motion.position.y - key.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(key.get_bounding_box().x, key.get_bounding_box().y);
    float my_r = std::max(prince->physics.scale.x, prince->physics.scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

bool CollisionManager::prince_collides_with_star(Prince *prince, Star &star)
{
    vec2 prince_bounding_box = prince->get_bounding_box();
    vec2 star_bounding_box = star.get_bounding_box();
    vec2 star_pos = star.get_position();

    float prince_off_set_x = prince_bounding_box.x / 2;
    float prince_off_set_y = prince_bounding_box.y / 2;

    float prince_right = prince->motion.position.x + prince_off_set_x;
    float prince_left = prince->motion.position.x - prince_off_set_x;
    float prince_top = prince->motion.position.y - prince_off_set_y;
    float prince_bottom = prince->motion.position.y + prince_off_set_y;

    float star_off_set_x = star_bounding_box.x / 2;
    float star_off_set_y = star_bounding_box.y / 2;

    float star_right = star_pos.x + star_off_set_x;
    float star_left = star_pos.x - star_off_set_x;
    float star_top = star_pos.y - star_off_set_y;
    float star_bottom = star_pos.y + star_off_set_y;

    if (prince_top <= star_bottom && prince_bottom >= star_top)
    {
        if (prince_left <= star_right && prince_right >= star_left)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool CollisionManager::prince_collides_with_wall(Prince *prince, Walls &wall, float ms)
{
    vec2 nvel = add(prince->m_velocity, mul(prince->motion.acceleration, ms / 1000));
    if (len(prince->motion.acceleration) > 0.001f)
    {
        if (len(nvel) > prince->motion.max_speed)
            nvel = mul(normalize(nvel), prince->motion.max_speed);
    }
    vec2 npos = add(prince->motion.position, mul(nvel, (ms / 1000)));

    vec2 prince_bounding_box = prince->get_bounding_box();
    vec2 wall_bounding_box = wall.get_bounding_box();
    vec2 wall_pos = wall.get_position();

    float prince_off_set_x = prince_bounding_box.x / 2;
    float prince_off_set_y = prince_bounding_box.y / 2;

    float prince_right = npos.x + prince_off_set_x;
    float prince_left = npos.x - prince_off_set_x;
    float prince_top = npos.y - prince_off_set_y;
    float prince_bottom = npos.y + prince_off_set_y;

    float wall_off_set_x = wall_bounding_box.x / 2;
    float wall_off_set_y = wall_bounding_box.y / 2;

    float wall_right = wall_pos.x + wall_off_set_x;
    float wall_left = wall_pos.x - wall_off_set_x;
    float wall_top = wall_pos.y - wall_off_set_y;
    float wall_bottom = wall_pos.y + wall_off_set_y;

    if (prince_top <= wall_bottom && prince_bottom >= wall_top)
    {
        if (prince_left <= wall_right && prince_right >= wall_left)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool CollisionManager::prince_collides_with_barrel(Prince *prince, Barrel &barrel, float ms)
{
    vec2 nvel = add(prince->m_velocity, mul(prince->motion.acceleration, ms / 1000));
    if (len(prince->motion.acceleration) > 0.001f)
    {
        if (len(nvel) > prince->motion.max_speed)
            nvel = mul(normalize(nvel), prince->motion.max_speed);
    }
    vec2 npos = add(prince->motion.position, mul(nvel, (ms / 1000)));

    vec2 prince_bounding_box = prince->get_bounding_box();
    vec2 barrel_bounding_box = barrel.get_bounding_box();
    vec2 barrel_pos = barrel.get_position();

    float prince_off_set_x = prince_bounding_box.x / 2;
    float prince_off_set_y = prince_bounding_box.y / 2;

    float prince_right = npos.x + prince_off_set_x;
    float prince_left = npos.x - prince_off_set_x;
    float prince_top = npos.y - prince_off_set_y;
    float prince_bottom = npos.y + prince_off_set_y;

    float barrel_off_set_x = barrel_bounding_box.x / 2;
    float barrel_off_set_y = barrel_bounding_box.y / 2;

    float barrel_right = barrel_pos.x + barrel_off_set_x;
    float barrel_left = barrel_pos.x - barrel_off_set_x;
    float barrel_top = barrel_pos.y - barrel_off_set_y;
    float barrel_bottom = barrel_pos.y + barrel_off_set_y;

    if (prince_top <= barrel_bottom && prince_bottom >= barrel_top)
    {
        if (prince_left <= barrel_right && prince_right >= barrel_left)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool CollisionManager::prince_collides_with_bullet(Prince *prince, Bullet *bullet)
{
    float dx = prince->motion.position.x - bullet->get_position().x;
    float dy = prince->motion.position.y - bullet->get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(bullet->get_bounding_box().x, bullet->get_bounding_box().y);
    float my_r = std::max(prince->physics.scale.x, prince->physics.scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

bool CollisionManager::bullet_collides_with_wall(Bullet *bullet, Walls &wall)
{
    float bullet_radius = fmax(bullet->get_bounding_box().x/2,bullet->get_bounding_box().y/2);
    float wall_radius = fmax(wall.get_bounding_box().x/2,wall.get_bounding_box().y/2);

    vec2 wall_pos = wall.get_position();
    vec2 bullet_pos = bullet->get_position();

    float distance = len(sub(wall_pos,bullet_pos));

    if (distance < bullet_radius+wall_radius)
        return true;
    else
        return false;
}

bool CollisionManager::bullet_collides_with_barrel(Bullet *bullet, Barrel &barrel)
{
    float bullet_radius = fmax(bullet->get_bounding_box().x/2,bullet->get_bounding_box().y/2);
    float barrel_radius = fmax(barrel.get_bounding_box().x/2,barrel.get_bounding_box().y/2);

    vec2 barrel_pos = barrel.get_position();
    vec2 bullet_pos = bullet->get_position();

    float distance = len(sub(barrel_pos,bullet_pos));

    if (distance < bullet_radius+barrel_radius)
        return true;
    else
        return false;
}

bool CollisionManager::bullet_collides_with_bullet(Bullet *bullet_e, Bullet &bullet_p)
{
    vec2 bullet_offset = mul(bullet_e->get_bounding_box(), 0.5);

    vec2 prince_bullet_top_left = sub(bullet_p.motion.position, bullet_offset);
    vec2 prince_bullet_bot_right = add(bullet_p.motion.position, bullet_offset);

    vec2 enemy_bullet_top_left = sub(bullet_e->motion.position, bullet_offset);
    vec2 enemy_bullet_bot_right = add(bullet_e->motion.position, bullet_offset);

    bool is_top = is_in_between(prince_bullet_top_left.y, enemy_bullet_bot_right.y, enemy_bullet_top_left.y);
    bool is_bottom = is_in_between(prince_bullet_bot_right.y, enemy_bullet_bot_right.y, enemy_bullet_top_left.y);
    bool is_left = is_in_between(prince_bullet_top_left.x, enemy_bullet_bot_right.x, enemy_bullet_top_left.x);
    bool is_right = is_in_between(prince_bullet_bot_right.x, enemy_bullet_bot_right.x, enemy_bullet_top_left.x);

    if ((is_left || is_right) && (is_bottom || is_top))
        return true;
    else
        return false;
}

bool CollisionManager::bullet_collides_with_boundry(Bullet *bullet)
{
    vec2 bullet_bounding_box = bullet->get_bounding_box();

    float bullet_off_set_x = bullet_bounding_box.x / 2;
    float bullet_off_set_y = bullet_bounding_box.y / 2;

    float bullet_right = bullet->motion.position.x + bullet_off_set_x;
    float bullet_left = bullet->motion.position.x - bullet_off_set_x;
    float bullet_top = bullet->motion.position.y - bullet_off_set_y;
    float bullet_bottom = bullet->motion.position.y + bullet_off_set_y;
    return bullet_top < 0 || bullet_bottom > 800 || bullet_left < 0 || bullet_right > 1200;
}

bool CollisionManager::bullet_collides_with_enemy(Bullet &bullet, Enemy &enemy)
{
    float distance = len(sub(bullet.motion.position, enemy.get_position()));
    float enemy_r = std::max(enemy.get_bounding_box().x, enemy.get_bounding_box().y);
    float bullet_r = std::max(bullet.physics.scale.x, bullet.physics.scale.y);
    float r = enemy_r + bullet_r;
    if (distance < r)
        return true;
    return false;
}
