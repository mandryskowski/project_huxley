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

typedef struct Mat3f
{
    float d[3][3];
} Mat3f;
Mat3f Mat3f_construct(Vec2f translation, Vec2f scale);
Mat3f Mat3f_multiply(Mat3f, Mat3f);
void Mat3f_print(Mat3f*);

Vec2i Vec2i_normalize(Vec2i);
Vec2i Vec2i_add(Vec2i, Vec2i);
Vec2i Vec2i_scale(Vec2i, float scalar);

Vec2f Vec2f_normalize(Vec2f);
Vec2f Vec2f_add(Vec2f, Vec2f);
Vec2f Vec2f_scale(Vec2f, float scalar);
#endif // MATH_H