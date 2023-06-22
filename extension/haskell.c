#include "haskell.h"
#include "entity.h"
#include "game_math.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "animation.h"
#include "audio.h"

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
    int num_of_projectiles = 50;
    for (int i = 0; i < num_of_projectiles; i++)
    {
        haskell->attack_velocity = Vec2d_rotate(haskell->attack_velocity, 360.0 / num_of_projectiles);
        shooter_spawn_attack(haskell);
    }
}

void spawn_monsters_attack(Entity *haskell, Entity *victim)
{
    haskell->attack_cooldown = 200;
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

bool haskell_jump_animation_update(Animation* anim)
{
    bool logic = animLogicDouble(anim);
    printf("yo %f %d\n", *(double*)anim->curVal, logic);
    if (logic)
        *((double*)anim->curVal) *= 0.5;
    if (*(double*)anim->curVal <= pow (0.5, 20.0)) // jump every 20 frames
    {
        *(double*)anim->curVal = 0.0;
        playSound(SOUND_DEATH_HEAVY);
    }
    return false;
}

Animation* Animation_construct_haskell_jump_animation(Entity* ent)
{
    Animation* anim = malloc(sizeof(Animation));
    anim->startVal = malloc(sizeof(double));
    anim->endVal = malloc(sizeof(double));
    *((double*)anim->startVal) = ent->renderOffset.y = 1.0;
    *((double*)anim->endVal) = 0.0;
    anim->framesLeftUntilUpdate = anim->framesPerUpdate = 1;

    anim->curVal = &ent->renderOffset.y;

    anim->animFunc = haskell_jump_animation_update;

    playSound(SOUND_DEATH_HEAVY);

    return anim;
}

bool haskell_attack(Entity *haskell, Entity *victim, AttackType type)
{
    switch (type) {
        case SPAWN_ENTITY:
            //printf("%d\n", attack_cycle_left);
            if (!attack_cycle_left)
            {
                haskell->currentAnimation = NULL;
                srand(clock());
                int p = rand() % 10;
                if (p < 2)
                {
                    attack = circle_attack;
                    attack_cycle_left = 6;
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

void haskell_death(Entity* attacker)
{
    // small amount of screen shake
    Player* playerData = ((Player*)attacker->room->entities[0]->specific_data);
    playerData->screenShakeFramesLeft = max(playerData->screenShakeFramesLeft, 75);
    attacker->room->entities[attacker->room->entity_cnt++] = construct_monster(attacker->pos, PORTAL, attacker->room);
    playSound(SOUND_HASKELL_DIES);
    playMusic(NO_SOUND, NO_SOUND);
}

void construct_haskell(Entity *monster)
{
    *monster = (Entity) {.ATK = 10, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-1, -1}, (Vec2d){1, 1}},
            .HP = 300, .maxHP = 300, .SPD = 2, .velocity = (Vec2d){0.0, 0.0}, .projectileStats = (ProjectileStats){0, 0},
            .attack_func = haskell_attack, .death_func = haskell_death, .faction = ENEMY, .attack_SPD = 4, .attack_cooldown = 20, .currentAnimation = NULL};
}