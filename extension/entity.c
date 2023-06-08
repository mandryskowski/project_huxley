#include "entity.h"

Entity Entity_construct()
{
    return (Entity) {.ATK = 1, .canFly = false,
            .hitbox = (Rectangle){(Vec2f){-1.0f, -1.0f}, (Vec2f){1.0f, 1.0f}},
     .HP = 100, .maxHP = 100,
     .pos = (Vec2f){1.5f, 1.5f}, .SPD = 1, .velocity = (Vec2f){0.0f, 0.0f}};
}

Entity Entity_construct_generic(Rectangle newHitbox, Vec2f newPos)
{
    return (Entity) {.ATK = 1, .canFly = false,
            .hitbox = newHitbox,
            .HP = 100, .maxHP = 100,
            .pos = newPos, .SPD = 1, .velocity = (Vec2f){0.0f, 0.0f}};
}