#ifndef ENTITY_H
#define ENTITY_H
#include "math.h"

typedef struct Entity
{
    Vec2f pos;
    Vec2f velocity;

    // relative to pos
    Vec2f hitboxBottomLeft;
    Vec2f hitboxTopRight;

    bool canFly;

    int HP, maxHP;
    float ATK;
    float SPD;

} Entity;

typedef struct Player
{
    Entity entity;
} Player;
#endif // ENTITY_H