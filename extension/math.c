#include "math.h"

Vec2f Vec2f_add(Vec2f lhs, Vec2f rhs)
{
    return (Vec2f){lhs.x + rhs.x, lhs.y + rhs.y};
}