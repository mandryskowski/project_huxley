#ifndef MATH_H
#define MATH_H

#include <stdbool.h>

#define EPS 1e-6
#define min(a, b) a < b ? a : b
//#define absGen(a) a < 0 ? -a : a

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

typedef struct Rectangle
{
    Vec2f bottomLeft, topRight;
} Rectangle;

Mat3f Mat3f_construct(Vec2f translation, Vec2f scale);
Mat3f Mat3f_multiply(Mat3f, Mat3f);
void Mat3f_print(Mat3f*);

Vec2i Vec2i_normalize(Vec2i);
Vec2i Vec2i_add(Vec2i, Vec2i);
Vec2i Vec2i_scale(Vec2i, float scalar);
bool Vec2i_equals(Vec2i, Vec2i);

bool Vec2f_zero(Vec2f);
Vec2f Vec2f_add(Vec2f, Vec2f);
Vec2f Vec2f_scale(Vec2f, float scalar);
void Vec2f_print(Vec2f);

Rectangle rectangle_Vec2f(Rectangle, Vec2f);
void Rectangle_print(Rectangle);
bool Vec2f_equals(Vec2f, Vec2f);

Vec2i Vec2f_to_Vec2i(Vec2f);
Vec2f Vec2i_to_Vec2f(Vec2i);

#endif // MATH_H
