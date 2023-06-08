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

Entity Entity_construct_generic(Rectangle, Vec2f);

typedef struct Player
{
    Entity entity;
} Player;
#endif // ENTITY_H