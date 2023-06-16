#include "haskell.h"
#include "entity.h"
#include "game_math.h"
#include <time.h>
#include <stdlib.h>
#include "animation.h"

int attack_cycle_left = 0;
typedef void (*current_attack)(Entity *, Entity *);
current_attack attack;

void lasers_attack(Entity *haskell, Entity *victim)
{
    haskell->attack_cooldown = 5;
    Vec2d temp = haskell->attack_velocity;
    haskell->attack_velocity = Vec2d_rotate(temp, 40 - attack_cycle_left);
    shooter_spawn_attack(haskell);
    haskell->attack_velocity = Vec2d_rotate(temp, -40 + attack_cycle_left);
    shooter_spawn_attack(haskell);
    haskell->attack_velocity = Vec2d_rotate(temp, 20 - attack_cycle_left % 40);
    shooter_spawn_attack(haskell);
    haskell->attack_velocity = temp;
}

void circle_attack(Entity *haskell, Entity *victim)
{
    haskell->attack_cooldown = 20;
    if (victim == victim->room->entities[0]) // if victim is the player
    {
        ((Player*)victim->specific_data)->screenShakeFramesLeft = 15;
    }

    if (attack_cycle_left == 6)
    {
        double new_pos_x = victim->pos.x > 7 ? victim->pos.x - 5 : victim->pos.x + 5;
        double new_pos_y = victim->pos.y > 7 ? victim->pos.y - 5 : victim->pos.y + 5;

        haskell->pos = (Vec2d){new_pos_x, new_pos_y};
        haskell->cooldown_left= 60;
        return;
    }
    int num_of_projectiles = 100;
    for (int i = 0; i < num_of_projectiles; i++)
    {
        haskell->attack_velocity = Vec2d_rotate(haskell->attack_velocity, 360.0 / num_of_projectiles);
        shooter_spawn_attack(haskell);
    }
}

void spawn_monsters_attack(Entity *haskell, Entity *victim)
{
    haskell->attack_cooldown = 120;
    Room *room = haskell->room;
    for (int i = -2; i <= 2; i += 4)
    {
        for (int j = -2; j <= 2; j += 4)
        {
            Vec2d new_vec = Vec2d_add(victim->pos, (Vec2d){i, j});
            if (new_vec.x > 2 && new_vec.y > 2 && new_vec.x < room->size.x - 2 && new_vec.y < room->size.y - 2)
            {
                room->entities[room->entity_cnt++] = construct_monster(new_vec, MINI_LAMBDA, room);
            }
        }
    }

}

bool haskell_attack(Entity *haskell, Entity *victim, AttackType type)
{
    switch (type) {
        case SPAWN_ENTITY:
            //printf("%d\n", attack_cycle_left);
            if (!attack_cycle_left)
            {
                srand(clock());
                int p = rand() % 10;
                printf("%d\n", p);
                
                if (p < 2)
                {
                    attack = circle_attack;
                    attack_cycle_left = 6;
                }
                else if (p < 6 && attack != spawn_monsters_attack)
                {
                    attack = spawn_monsters_attack;
                    attack_cycle_left = 1;
                }
                else
                {
                    attack = lasers_attack;
                    attack_cycle_left = 50;
                }
            }
            attack(haskell, victim);
            attack_cycle_left--;
            return true;
        default:
            return false;
    }
}

void construct_haskell(Entity *monster)
{
    *monster = (Entity) {.ATK = 10, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-1, -1}, (Vec2d){1, 1}},
            .HP = 300, .maxHP = 300, .SPD = 2, .velocity = (Vec2d){0.0, 0.0},
            .attack_func = haskell_attack, .faction = ENEMY, .attack_SPD = 6, .attack_cooldown = 20, .currentAnimation = NULL};
}