#include "entity.h"

Entity Entity_construct()
{
    return (Entity) {.ATK = 1, .canFly = false,
     .hitboxBottomLeft = (Vec2f){-1.0f, -1.0f}, .hitboxTopRight = (Vec2f){1.0f, 1.0f},
     .HP = 100, .maxHP = 100,
     .pos = (Vec2f){1.5f, 1.5f}, .SPD = 1, .velocity = (Vec2f){0.0f, 0.0f}};
}