#pragma once

#include <set>
#include <vector>
#include <iostream>
#include "common.hpp"
#include "prince.hpp"
#include "enemy.hpp"
#include "bullet.hpp"
#include "barrel.hpp"

class CollisionManager
{
public:
	bool static prince_collides_with_enemy(Prince *prince, Enemy *enemy);

	bool static prince_collides_with_portal(Prince *prince, Portal &portal);

	bool static prince_collides_with_heart(Prince *prince, Heart &heart);

	bool static prince_collides_with_key(Prince *prince, Key &key);

	bool static prince_collides_with_star(Prince *prince, Star &star);

	bool static prince_collides_with_wall(Prince *prince, Walls &walls, float ms);

    bool static prince_collides_with_barrel(Prince *prince, Barrel &barrel, float ms);

	bool static prince_collides_with_bullet(Prince *prince, Bullet *bullet);

	bool static bullet_collides_with_wall(Bullet *bullet, Walls &walls);

    bool static bullet_collides_with_barrel(Bullet *bullet, Barrel &barrel);

    bool static bullet_collides_with_bullet(Bullet *bullet_e, Bullet &bullet_p);

	bool static bullet_collides_with_boundry(Bullet *bullet);

	bool static bullet_collides_with_enemy(Bullet &bullet, Enemy &enemy);
};