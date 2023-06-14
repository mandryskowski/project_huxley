#include "entity.h"
#include "movement.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
        //printf("%d post %p %p\n", type, attacker->attack_func, zombie_attack);
        if (attacker->attack_func(attacker, victim, type))
        {
            attacker->cooldown_left = attacker->attack_cooldown;
        }
    }
}

void zombie_collision_attack(Entity *attacker, Entity *victim)
{
    victim->HP -= attacker->ATK;
}

bool zombie_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case ATTACK_CONTACT:
            zombie_collision_attack(attacker, victim);
            return true;
        default:
            return false;
    }
}

void projectile_collision_attack(Entity *attacker, Entity *victim)
{
    victim->HP -= attacker->ATK;
    killEntity(attacker);
}

bool projectile_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case ATTACK_CONTACT:
            projectile_collision_attack(attacker, victim);
            return true;
        default:
            return false;
    }
}

bool mine_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case ATTACK_CONTACT:
            killEntity(attacker);
            return true;
        case SPAWN_ENTITY:
            killEntity(attacker);
            return true;
        default:
            return false;
    }
}

Entity construct_projectile(Entity *creator)
{
    return (Entity) {.ATK = creator->ATK, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.1, -0.1}, (Vec2d){0.1, 0.1}},
            .HP = INT_MAX, .maxHP = INT_MAX,
            .pos = (Vec2d)creator->pos, .SPD = 5, .velocity = creator->attack_velocity, .attack_func = projectile_attack, .faction = creator->faction, .room = creator->room, .textureID = creator->faction == ALLY ? 0 : 5};
}


void shooter_spawn_attack(Entity *attacker)
{
    Entity *projectile = malloc(sizeof(Entity));
    *projectile = construct_projectile(attacker);
    attacker->room->entities[attacker->room->entity_cnt++] = projectile;
}

bool shooter_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case SPAWN_ENTITY:
            shooter_spawn_attack(attacker);
            return true;
        default:
            return false;
    }
}

void mine_death(Entity *attacker)
{
    Rectangle mine_hitbox = (Rectangle){{attacker->pos.x - 1, attacker->pos.y - 1}, {attacker->pos.x + 2, attacker->pos.y + 2}};
    for (Entity **entity = attacker->room->entities; *entity; entity++)
    {
        Vec2d colResult = detectCollisionRect(mine_hitbox, rectangle_Vec2d((*entity)->hitbox, (*entity)->pos));
        if (colResult.x > 0 && colResult.y > 0)
        {
            (*entity)->HP -= attacker->ATK;
        }
    }
}

Entity construct_mine(Entity *creator)
{
    return (Entity) {.ATK = 90, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.1, -0.1}, (Vec2d){0.1, 0.1}}, .attack_SPD = 1, .attack_velocity = (Vec2d){1, 1},
            .HP = INT_MAX, .maxHP = INT_MAX, .death_func = mine_death,
            .pos = (Vec2d)creator->pos, .SPD = 0, .velocity = {0, 0}, .attack_func = mine_attack, .faction = creator->faction, .room = creator->room, .cooldown_left = 300, .textureID = 0};
}

void spawn_mine(Entity *attacker)
{
    Entity *mine = malloc(sizeof(Entity));
    *mine = construct_mine(attacker);
    attacker->room->entities[attacker->room->entity_cnt++] = mine;
}

bool bomber_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case SPAWN_ENTITY:
            spawn_mine(attacker);
            return true;
        default:
            return false;
    }
}

void construct_bomber(Entity *monster)
{
    *monster =  (Entity) {.ATK = 0, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.2, -0.2}, (Vec2d){0.2, 0.2}}, .attack_velocity = (Vec2d){1,1}, .attack_SPD = 1.0,
            .HP = 1, .maxHP = 1,
            .SPD = 3, .velocity = (Vec2d){0, 0}, .attack_func = bomber_attack, .faction = ENEMY, .attack_cooldown = 60, .cooldown_left = 0, .textureID = 3};
}

void construct_zombie(Entity *monster)
{
    *monster =  (Entity) {.ATK = 3, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.4, -0.4}, (Vec2d){0.4, 0.4}},
            .HP = 100, .maxHP = 100,
            .SPD = 2, .velocity = (Vec2d){0.0, 0.0}, .attack_func = zombie_attack, .faction = ENEMY, .attack_cooldown = 120, .cooldown_left = 0, .textureID = 3};
}

void construct_shooter(Entity *monster)
{
    *monster =  (Entity) {.ATK = 1, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.4, -0.4}, (Vec2d){0.4, 0.4}},
            .HP = 60, .maxHP = 60,
            .SPD = 2, .velocity = (Vec2d){0.0, 0.0}, .attack_func = shooter_attack, .faction = ENEMY, .attack_cooldown = 30, .attack_SPD = 10, .attack_velocity = {0, 0}, .textureID = 3};
}

void construct_flying_shooter(Entity *monster)
{
    *monster =  (Entity) {.ATK = 1, .canFly = true,
            .hitbox = (Rectangle){(Vec2d){-0.4, -0.4}, (Vec2d){0.4, 0.4}},
            .HP = 60, .maxHP = 60,
            .SPD = 3, .velocity = (Vec2d){0.0, 0.0}, .attack_func = shooter_attack, .faction = ENEMY, .attack_cooldown = 10, .attack_SPD = 6, .attack_velocity = {0, 0}, .textureID = 1};
}

Player *Entity_construct_player()
{
    Player *player = calloc(sizeof(Player), 1);
    Entity *entity = calloc(sizeof(Entity), 1);

    *entity = (Entity) {.ATK = 100, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.25, -0.25}, (Vec2d){0.25, 0.25}},
            .HP = 100, .maxHP = 100, .SPD = 5, .velocity = (Vec2d){0.0, 0.0},
            .attack_func = shooter_attack, .faction = ALLY, .attack_SPD = 5, .attack_cooldown = 30, .textureID = 2};
    *player = (Player) {.entity = entity, .movement_swing = 0.3, .acceleration_const = 0.8, .cameraSize = (Vec2d){8, 8}};

    return player;
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
        case BOMBER:
            construct_bomber(monster);
            break;
        default:
            perror("not a monster mf\n");
            exit(0);
    }

    monster->room = room;
    monster->pos = pos;

    return monster;
}

bool isMine(Entity *entity)
{
    return fabs(entity->SPD) < EPSILON;
}

