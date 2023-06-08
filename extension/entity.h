#ifndef ENTITY_H
#define ENTITY_H
#include "math.h"
#include <stdbool.h>

typedef struct Entity
{
    Vec2f pos;
    Vec2f velocity;

    // relative to pos
    Rectangle hitbox;

    bool canFly;

    int HP, maxHP;
    float ATK;
    float SPD;

} Entity;

Entity Entity_construct();

typedef struct Player
{
    Entity entity;
} Player;
#endif // ENTITY_H