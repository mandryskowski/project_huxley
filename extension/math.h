#ifndef MATH_H
#define MATH_H

typedef struct Vec2i
{
    int x, y;
} Vec2i;

typedef struct Vec2f
{
    float x, y;
} Vec2f;

Vec2f normalize(Vec2f);
Vec2f Vec2f_add(Vec2f, Vec2f);
#endif // MATH_H