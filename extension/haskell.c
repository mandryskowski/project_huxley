#include "haskell.h"
#include "entity.h"
#include "game_math.h"
#include <stdlib.h>

void construct_haskell(Entity *monster)
{
    *monster = (Entity) {.ATK = 10, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-1, -1}, (Vec2d){1, 1}},
            .HP = 300, .maxHP = 300, .SPD = 2, .velocity = (Vec2d){0.0, 0.0},
            .attack_func = circle_attack, .faction = ENEMY, .attack_SPD = 3, .attack_cooldown = 20};
}

void circle_attack(Entity *haskell)
{
    haskell->attack_velocity = (Vec2d){3, 0};
    const int num_of_projetiles = 50;
    for (int i = 0; i < num_of_projetiles; i++)
    {
        haskell->attack_velocity = Vec2d_rotate(haskell->attack_velocity, 180.0 / num_of_projetiles);
        handle_attack(haskell, NULL, SPAWN_ENTITY);
    }
}