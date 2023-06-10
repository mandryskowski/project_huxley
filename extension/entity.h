#ifndef ENTITY_H
#define ENTITY_H
#include "math.h"
#include <stdbool.h>

typedef struct Entity
{
    Vec2d pos;
    Vec2d velocity;

    // relative to pos
    Rectangle hitbox;

    bool canFly;

    int HP, maxHP;
    int ATK;
    double SPD;

} Entity;

Entity Entity_construct();

Entity Entity_construct_generic(Rectangle, Vec2d);

typedef struct Player
{
    Entity entity;
    double acceleration_const; // between 0 and 1
} Player;
#endif // ENTITY_H