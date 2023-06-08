#include "math.h"
#include <math.h>
#include <stdlib.h>

Mat3f Mat3f_construct(Vec2f translation, Vec2f scale)
{
    Mat3f mat;
    memset(&mat.d, 0, sizeof(float) * 3 * 3); // fill matrix with 0s
    
    mat.d[0][0] = scale.x;
    mat.d[1][1] = scale.y;

    mat.d[2][0] = scale.x * translation.x;
    mat.d[2][1] = scale.y * translation.y;
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

Vec2i Vec2i_normalize(Vec2i vec)
{
    return Vec2i_scale(vec, sqrt(vec.x * vec.x + vec.y * vec.y));
}

Vec2i Vec2i_add(Vec2i lhs, Vec2i rhs)
{
    return (Vec2i){lhs.x + rhs.x, lhs.y + rhs.y};
}

Vec2i Vec2i_scale(Vec2i vec, float scalar)
{
    return (Vec2i) {vec.x * scalar, vec.y * scalar};
}

Vec2f Vec2f_normalize(Vec2f vec)
{
    return Vec2f_scale(vec, sqrt(vec.x * vec.x + vec.y * vec.y));
}

Vec2f Vec2f_add(Vec2f lhs, Vec2f rhs)
{
    return (Vec2f){lhs.x + rhs.x, lhs.y + rhs.y};
}

Vec2f Vec2f_scale(Vec2f vec, float scalar)
{
    return (Vec2f) {vec.x * scalar, vec.y * scalar};
}