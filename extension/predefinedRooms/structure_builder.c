#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../room.h"
#include "../entity.h"
#include "../game_math.h"
#include "structure_builder.h"

void patternBuilder(TileType** tiles,Pattern pattern, Vec2i topLeft, Vec2i bottomRight, TileType type)
{
    int xMin = min(topLeft.x, bottomRight.x); 
    int xMax = max(topLeft.x, bottomRight.x);
    int yMin = min(topLeft.y, bottomRight.y);
    int yMax = max(topLeft.y, bottomRight.y);
    //printf("xMin is: %d\nxMax is:")

    if(pattern == FILL)
    {
        for(int i = xMin; i <= xMax; i++)
        {
            for(int j = yMin; j <= yMax; j++)
            {
                tiles[i][j] = type;
            }
        }
    }

    if(pattern == CHECKERED)
    {
        for(int i = xMin; i <= xMax; i += 2)
        {
            for(int j = yMin; j <= yMax; j += 2)
            {
                tiles[i][j] = type;
            }
        }
    }
}

void putLshape(TileType** tiles, Vec2i topLeft, Vec2i bottomRight, TileType type) 
{
    Vec2i bottomLeft;
    bottomLeft.x = topLeft.x;
    bottomLeft.y = bottomRight.y;

    patternBuilder(tiles, FILL, topLeft, bottomLeft, type);
    patternBuilder(tiles, FILL, bottomLeft, bottomRight, type);
}

void putUshape(TileType** tiles, Vec2i topLeft, Vec2i bottomRight, TileType type) 
{
    Vec2i bottomLeft;
    bottomLeft.x = topLeft.x;
    bottomLeft.y = bottomRight.y;

    Vec2i topRight;
    topRight.x = bottomRight.x;
    topRight.y = topLeft.y;

    patternBuilder(tiles, FILL, topLeft, bottomLeft, type);
    patternBuilder(tiles, FILL, bottomLeft, bottomRight, type);
    patternBuilder(tiles, FILL, bottomRight, topRight, type);
}

void putTshape(TileType** tiles, Vec2i topLeft, Vec2i bottomRight, TileType type) 
{
    Vec2i bottomLeft;
    bottomLeft.x = topLeft.x;
    bottomLeft.y = bottomRight.y;

    Vec2i topRight;
    topRight.x = bottomRight.x;
    topRight.y = topLeft.y;

    Vec2i topMid = Vec2i_middle(topLeft, topRight);
    Vec2i bottomMid = Vec2i_middle(bottomLeft, bottomRight);

    patternBuilder(tiles, FILL, topLeft, topRight, type);
    patternBuilder(tiles, FILL, topMid, bottomMid, type);
}

void presetStructures(TileType** tiles, Structure structure, Vec2i topLeft, Vec2i bottomRight, TileType type, int angle)
{
    int swapper;
    if(angle % 360 >= 90)
    {
        swapper = topLeft.x;
        topLeft.x = bottomRight.x;
        bottomRight.x = swapper;
    }
    if(angle % 360 >= 180)
    {
        swapper = topLeft.y;
        topLeft.y = bottomRight.y;
        bottomRight.y = swapper;
    }
    if(angle % 360 >= 270)
    {
        swapper = topLeft.x;
        topLeft.x = bottomRight.x;
        bottomRight.x = swapper;
    }

    switch(structure) 
    {
        case L_SHAPE:
            putLshape(tiles, topLeft, bottomRight, type);
            break;
        case U_SHAPE:
            putUshape(tiles, topLeft, bottomRight, type);
            break;
        case T_SHAPE:
            putTshape(tiles, topLeft, bottomRight, type);
            break;        
    }
}