#include "java.h"
#include "game_math.h"
#include "util.h"
#include "room.h"
#include "movement.h"
#include <stdlib.h>

static int attack_cycle_left = 0;
static int meteor_type;
typedef void (*current_attack)(Entity *, Entity *);
current_attack attack;

void spawn_meteor(Vec2d pos, Room *room, int type)
{
    int num_of_affected_tiles = 5 + rand() % 3;
    Vec2i *affected_tiles = calloc(sizeof(Vec2i), num_of_affected_tiles);
    Vec2i *affected_tiles_cpy = affected_tiles;
    *affected_tiles = Vec2d_to_Vec2i(pos);
    Vec2i *affected_tiles_end = affected_tiles + 1;
    Vec2i dirs[4] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

    for (int i = 0; i < num_of_affected_tiles; i++)
    {
        shuffle(affected_tiles, affected_tiles_end - affected_tiles, sizeof(Vec2i));
        for (int j = 0; j < 4; j++)
        {
            Vec2i new_coord = Vec2i_add(*affected_tiles, dirs[j]);
            if (new_coord.x < 0 || new_coord.x == room->size.x || new_coord.y < 0 || new_coord.y == room->size.y)
            {
                continue;
            }
            bool not_visited = true;
            for (Vec2i *tile = affected_tiles_cpy; tile < affected_tiles_end; tile++)
            {
                if (tile->x == new_coord.x && tile->y == new_coord.y)
                {
                    not_visited = false;
                    break;
                }
            }
            if (not_visited)
                *affected_tiles_end++ = new_coord;
        }

        affected_tiles++;
    }

    for (Vec2i *tile = affected_tiles_cpy; tile < affected_tiles; tile++)
    {
        room->tiles[tile->x][tile->y].tile_effect = type ? (TileEffect){on_fire, 600} : (TileEffect){frozen, 600};
    }

    Rectangle meteor_hitbox = (Rectangle){{pos.x - 1.5, pos.y - 1.5}, {pos.x + 1.5, pos.y + 1.5}};
    Vec2d colResult = detectCollisionRect(meteor_hitbox, rectangle_Vec2d((*room->entities)->hitbox, (*room->entities)->pos));
    if (colResult.x > 0 && colResult.y > 0)
    {
        take_dmg(*room->entities, 30);
    }

    free(affected_tiles);
}

void summon_meteors(Entity *java, Entity *victim)
{
    const int animation_time = 40;
    if (attack_cycle_left == 300)
    {
        meteor_type = rand() % 2;
    }

    if (attack_cycle_left % 30 == 0 && attack_cycle_left)
    {
        Vec2d player_pred = Vec2d_add(victim->pos, Vec2d_scale(victim->velocity, animation_time / 60));
        player_pred = Vec2d_add(player_pred, (Vec2d){(double)(rand() % 11) / 10 - 0.5, -(double)(rand() % 4) / 11 -0.5});
        Vec2d meteor_pos = Vec_2d_bound(player_pred, victim->room->size);
        spawn_meteor(meteor_pos, victim->room, meteor_type);
    }
}

bool java_attack(Entity *haskell, Entity *victim, AttackType type)
{
    switch (type) {
        case SPAWN_ENTITY:
            //printf("%d\n", attack_cycle_left);
            if (!attack_cycle_left)
            {
                haskell->currentAnimation = NULL;
                int p = rand() % 10;
                if (p < 2)
                {
                    attack = summon_meteors;
                    attack_cycle_left = 300;
                    haskell->currentAnimation = Animation_construct_haskell_jump_animation(haskell);
                }
                else if (p < 6)
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

void java_death(Entity* attacker)
{
    // small amount of screen shake
    Player* playerData = ((Player*)attacker->room->entities[0]->specific_data);
    playerData->screenShakeFramesLeft = max(playerData->screenShakeFramesLeft, 75);
    attacker->room->entities[attacker->room->entity_cnt++] = construct_monster(attacker->pos, PORTAL, attacker->room);
    playSound(SOUND_HASKELL_DIES);
    playMusic(NO_SOUND, NO_SOUND);
}

void construct_java(Entity *monster)
{
    *monster = (Entity) {.ATK = 10, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-1, -1}, (Vec2d){1, 1}},
            .HP = 300, .maxHP = 300, .SPD = 2, .velocity = (Vec2d){0.0, 0.0}, .projectileStats = (ProjectileStats){0, 0},
            .attack_func = java_attack, .death_func = java_death, .faction = ENEMY, .attack_SPD = 6, .attack_cooldown = 20, .currentAnimation = NULL};
}