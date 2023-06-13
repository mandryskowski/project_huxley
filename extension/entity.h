#ifndef ENTITY_H
#define ENTITY_H

#include "game_math.h"
#include "room.h"
#include <stdbool.h>

typedef struct Entity Entity;

typedef enum {
    ATTACK_CONTACT, SPAWN_PROJECTILE
} AttackType;

typedef enum {
    ALLY, ENEMY
} Faction;

typedef enum {
    ZOMBIE, SHOOTER, FLYING_SHOOTER, NOT_MONSTER
} MonsterType;

typedef bool (*Attack_Func)(Entity *, Entity *, AttackType);

typedef struct Entity
{
    Vec2d pos;
    Vec2d velocity;
    Vec2d attack_velocity;

    // relative to pos
    Rectangle hitbox;

    bool canFly;

    int HP, maxHP, attack_cooldown, cooldown_left;
    int ATK;
    double SPD, attack_SPD;

    Faction faction;
    Attack_Func attack_func;

    Room *room;
} Entity;

typedef struct Player
{
    Entity *entity;
    double acceleration_const; // between 0 and 1
    double movement_swing; // between 0 and 1
} Player;

Entity *construct_monster(Vec2d pos, MonsterType type, Room *room);
Player *Entity_construct_player();

Entity Entity_construct_generic(Rectangle, Vec2d);

bool isProjectile(Entity *);

bool isDead(Entity *);

void killEntity(Entity *);

void handle_attack(Entity *, Entity *, AttackType);

#endif // ENTITY_H