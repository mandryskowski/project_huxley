#include "entity.h"

Entity Entity_construct()
{
    return (Entity) {.ATK = 1, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.5f, -0.5f}, (Vec2d){0.5f, 0.5f}},
     .HP = 100, .maxHP = 100,
     .pos = (Vec2d){4.0f, 4.0f}, .SPD = 1, .velocity = (Vec2d){0.0f, 0.0f}};
}

Entity Entity_construct_generic(Rectangle newHitbox, Vec2d newPos)
{
    return (Entity) {.ATK = 1, .canFly = false,
            .hitbox = newHitbox,
            .HP = 100, .maxHP = 100,
            .pos = newPos, .SPD = 1, .velocity = (Vec2d){0.0f, 0.0f}};
}