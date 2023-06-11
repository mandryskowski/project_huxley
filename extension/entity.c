#include "entity.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

void construct_zombie(Entity *monster)
{
    *monster =  (Entity) {.ATK = 2, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.4f, -0.4f}, (Vec2d){0.4f, 0.4f}},
     .HP = 100, .maxHP = 100,
     .SPD = 3, .velocity = (Vec2d){0.0f, 0.0f}, .attack_func = zombie_attack, .faction = ENEMY, .attack_cooldown = 120};
}

void construct_shooter(Entity *monster)
{
    *monster =  (Entity) {.ATK = 1, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.4f, -0.4f}, (Vec2d){0.4f, 0.4f}},
            .HP = 60, .maxHP = 60,
            .SPD = 2, .velocity = (Vec2d){0.0f, 0.0f}, .attack_func = shooter_attack, .faction = ENEMY, .attack_cooldown = 120, .attack_SPD = 10, .attack_velocity = {0, 0}};
}

void construct_flying_shooter(Entity *monster)
{
    *monster =  (Entity) {.ATK = 1, .canFly = true,
            .hitbox = (Rectangle){(Vec2d){-0.4f, -0.4f}, (Vec2d){0.4f, 0.4f}},
            .HP = 60, .maxHP = 60,
            .SPD = 4, .velocity = (Vec2d){0.0f, 0.0f}, .attack_func = shooter_attack, .faction = ENEMY, .attack_cooldown = 10, .attack_SPD = 6, .attack_velocity = {0, 0}};
}

Player *Entity_construct_player()
{
    Player *player = calloc(sizeof(Player), 1);
    Entity *entity = calloc(sizeof(Entity), 1);

    *entity = (Entity) {.ATK = 1, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.4f, -0.4f}, (Vec2d){0.4f, 0.4f}},
            .HP = 100, .maxHP = 100, .SPD = 5, .velocity = (Vec2d){0.0f, 0.0f},
            .attack_func = shooter_attack, .faction = ALLY, .attack_SPD = 5, .attack_cooldown = 30};
    *player = (Player) {.entity = entity, .movement_swing = 0.3, .acceleration_const = 0.8};

    return player;
}

Entity construct_projectile(Entity *creator)
{
    return (Entity) {.ATK = 10, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.1f, -0.1f}, (Vec2d){0.1f, 0.1f}},
            .HP = INT_MAX, .maxHP = INT_MAX,
            .pos = (Vec2d)creator->pos, .SPD = 5, .velocity = creator->attack_velocity, .attack_func = projectile_attack, .faction = creator->faction};
}

Entity *construct_monster(Vec2d pos, MonsterType type, Room *room)
{
    Entity *monster = calloc(1, sizeof(Entity));

    switch (type)
    {
        case ZOMBIE:
            construct_zombie(monster);
            break;
        case SHOOTER:
            construct_shooter(monster);
            break;
        case FLYING_SHOOTER:
            construct_flying_shooter(monster);
            break;
        default:
            perror("not a monster mf\n");
            exit(0);
    }

    monster->room = room;
    monster->pos = pos;

    return monster;
}

bool isProjectile(Entity *entity)
{
    return entity->maxHP == INT_MAX;
}

bool isDead(Entity *entity)
{
    return entity->HP <= 0;
}

void killEntity(Entity *entity)
{
    entity->HP = -1;
}

void handle_attack(Entity *attacker, Entity *victim, AttackType type)
{
    if (attacker->cooldown_left)
    {
        return;
    }
    if (victim == NULL || (attacker->faction != victim->faction &&
            (!isProjectile(attacker) || !isProjectile(victim))))
    {
        attacker->attack_func(attacker, victim, type);
        attacker->cooldown_left = attacker->attack_cooldown;
    }
}

void zombie_collision_attack(Entity *attacker, Entity *victim)
{
    victim->HP -= attacker->ATK;
}

void zombie_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case ATTACK_CONTACT:
            zombie_collision_attack(attacker, victim);
            break;
        default:
            break;
    }
}

void projectile_collision_attack(Entity *attacker, Entity *victim)
{
    victim->HP -= attacker->ATK;
    killEntity(attacker);
}

void projectile_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case ATTACK_CONTACT:
            projectile_collision_attack(attacker, victim);
            break;
        default:
            break;
    }
}

void shooter_spawn_attack(Entity *attacker)
{
    Entity *projectile = malloc(sizeof(Entity));
    *projectile = construct_projectile(attacker);
    attacker->room->entities[attacker->room->entity_cnt++] = projectile;
}

void shooter_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case SPAWN_PROJECTILE:
            shooter_spawn_attack(attacker);
            break;
        default:
            break;
    }
}

