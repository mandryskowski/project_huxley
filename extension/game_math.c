#include "game_math.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

Mat3f Mat3f_construct(Vec2d translation, Vec2d scale)
{
    Mat3f mat;
    memset(&mat.d, 0, sizeof(float) * 3 * 3); // fill matrix with 0s

    mat.d[0][0] = scale.x;
    mat.d[1][1] = scale.y;

    mat.d[2][0] = translation.x;
    mat.d[2][1] = translation.y;
    mat.d[2][2] = 1;

    return mat;
}

Mat3f Mat3f_multiply(Mat3f lhs, Mat3f rhs)
{
    Mat3f out;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            out.d[i][j] = 0;
            for (int k = 0; k < 3; k++)
            {
                out.d[i][j] += lhs.d[i][k] * rhs.d[k][j];
            }
        }
    }

    return out;
}

void Mat3f_print(Mat3f* mat)
{
    printf("Mat3 print\n");
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            printf("%f ", mat->d[x][y]);
        }
        printf("\n");
    }
}

//Vec2i Vec2i_normalize(Vec2i vec)
//{
//    return Vec2i_scale(vec, sqrt(vec.x * vec.x + vec.y * vec.y));
//}

Vec2i Vec2i_add(Vec2i lhs, Vec2i rhs)
{
    return (Vec2i){lhs.x + rhs.x, lhs.y + rhs.y};
}

Vec2i Vec2i_scale(Vec2i vec, float scalar)
{
    return (Vec2i) {vec.x * scalar, vec.y * scalar};
}

Vec2d Vec2d_rotate(Vec2d vec, double angle)
{
    angle = PI * angle / 180;
    return (Vec2d){vec.x * cos(angle) - vec.y * sin(angle), vec.x * sin(angle) + vec.y * cos(angle)};
}

Vec2d Vec2d_normalize(Vec2d vec)
{
    if (Vec2d_zero(vec))
    {
        return vec;
    }
    return Vec2d_scale(vec, 1.0 / sqrt(vec.x * vec.x + vec.y * vec.y));
}

bool Vec2i_equals(Vec2i lhs, Vec2i rhs)
{
    return (lhs.x == rhs.x && lhs.y == rhs.y);
}
bool Vec2d_zero(Vec2d vec)
{
    return fabs(vec.x) < EPSILON && fabs(vec.y) < EPSILON;
}

Vec2d Vec2d_add(Vec2d lhs, Vec2d rhs)
{
    return (Vec2d){lhs.x + rhs.x, lhs.y + rhs.y};
}

Vec2d Vec2d_scale(Vec2d vec, double scalar)
{
    return (Vec2d) {vec.x * scalar, vec.y * scalar};
}

void Vec2d_print(Vec2d vec)
{
    printf("(%f, %f)", vec.x, vec.y);
}

Rectangle rectangle_Vec2d(Rectangle rect, Vec2d vec)
{
    return (Rectangle){Vec2d_add(rect.bottomLeft, vec), Vec2d_add(rect.topRight, vec)};
}

void Rectangle_print(Rectangle rect)
{
    Vec2d_print(rect.bottomLeft);
    printf(" ");
    Vec2d_print(rect.topRight);
}

bool Vec2d_equals(Vec2d lhs, Vec2d rhs)
{
    return (fabs(lhs.x - rhs.x) < EPSILON && fabs(lhs.y - rhs.y) < EPSILON);
}

Vec2i Vec2d_to_Vec2i(Vec2d vec)
{
    return (Vec2i){floor(vec.x), floor(vec.y)};
}

Vec2d Vec2i_to_Vec2d(Vec2i vec)
{
    return (Vec2d){(double)vec.x, (double)vec.y};
}
