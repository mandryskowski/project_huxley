#ifndef GAME_MATH_H
#define GAME_MATH_H

#include <stdbool.h>

#define EPSILON 1e-10
#define PI 3.14159265359
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

//#define absGen(a) a < 0 ? -a : a

typedef struct Vec2i
{
    int x, y;
} Vec2i;

typedef struct Vec2d
{
    double x, y;
} Vec2d;

typedef struct Vec4d
{
    double x, y, z, w;
} Vec4d;

typedef struct Vec4f
{
    float x, y, z, w;
} Vec4f;

typedef struct Mat3f
{
    float d[3][3];
} Mat3f;

typedef struct Rectangle
{
    Vec2d bottomLeft, topRight;
} Rectangle;

double clamp(double a, double minVal, double maxVal);

Mat3f Mat3f_construct(Vec2d translation, Vec2d scale);
Mat3f Mat3f_multiply(Mat3f, Mat3f);
void Mat3f_print(Mat3f*);

Vec2i Vec2i_normalize(Vec2i);
Vec2i Vec2i_add(Vec2i, Vec2i);
Vec2i Vec2i_scale(Vec2i, float scalar);
bool Vec2i_equals(Vec2i, Vec2i);

bool Vec2d_zero(Vec2d);
Vec2d Vec2d_rotate(Vec2d, double);
Vec2d Vec2d_normalize(Vec2d);
Vec2d Vec2d_add(Vec2d, Vec2d);
Vec2d Vec2d_scale(Vec2d, double scalar);
void Vec2d_print(Vec2d);

Rectangle rectangle_Vec2d(Rectangle, Vec2d);
void Rectangle_print(Rectangle);
bool Vec2d_equals(Vec2d, Vec2d);

Vec2i Vec2d_to_Vec2i(Vec2d);
Vec2d Vec2i_to_Vec2d(Vec2i);

double Vec2d_metric_distance(Vec2d lhs, Vec2d rhs);
double angle_between_Vec2d(Vec2d a, Vec2d b);
Vec2i Vec2i_middle(Vec2i lhs, Vec2i rhs);

Vec4d Vec4d_add(Vec4d, Vec4d);
Vec4d Vec4d_scale(Vec4d, double scalar);
Vec4d Vec4d_lerp(Vec4d lhs, Vec4d rhs, double T); // T from 0 to 1. If 0, returns lhs. If 1, returns rhs. Otherwise, returns a mix based on T.

Vec4f Vec4d_to_Vec4f(Vec4d);
#endif // GAME_MATH_H
