#include "entity.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

Entity Entity_construct_zombie()
{
    return (Entity) {.ATK = 1, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.4f, -0.4f}, (Vec2d){0.4f, 0.4f}},
     .HP = 100, .maxHP = 100,
     .pos = (Vec2d){4.0f, 4.0f}, .SPD = 1, .velocity = (Vec2d){0.0f, 0.0f}, .attack_func = zombie_attack, .faction = ENEMY, .attack_cooldown = 10};
}

Entity Entity_construct_player()
{
    return (Entity) {.ATK = 1, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.4f, -0.4f}, (Vec2d){0.4f, 0.4f}},
            .HP = 100, .maxHP = 100,
            .pos = (Vec2d){4.0f, 4.0f}, .SPD = 1, .velocity = (Vec2d){0.0f, 0.0f}, .attack_func = shooter_attack, .faction = ALLY, .attack_SPD = 5, .attack_cooldown = 60};
}

Entity construct_projectile(Entity *creator)
{
    return (Entity) {.ATK = 10, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.1f, -0.1f}, (Vec2d){0.1f, 0.1f}},
            .HP = INT_MAX, .maxHP = INT_MAX,
            .pos = (Vec2d)creator->pos, .SPD = 5, .velocity = creator->attack_velocity, .attack_func = projectile_attack, .faction = creator->faction};
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

