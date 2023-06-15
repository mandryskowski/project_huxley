#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../room.h"
#include "../game_math.h"
#include "structure_builder.h"

void patternBuilder(Room *room, Pattern pattern, Vec2i topLeft, Vec2i bottomRight, TileType type)
{
    int xMin = min(topLeft.x, bottomRight.x); 
    int xMax = max(topLeft.x, bottomRight.x);
    int yMin = min(topLeft.y, bottomRight.y);
    int yMax = max(topLeft.y, bottomRight.y);


    if(pattern == FILL)
    {
        for(int i = xMin; i <= xMax; i++)
        {
            for(int j = yMin; j <= yMax; j++)
            {
                room->tiles[j][i].type = type;
            }
        }
    }
    
   
    if(pattern == CHECKERED)
    {
        for(int i = xMin; i < xMax; i += 2)
        {
            for(int j = yMin; j < yMax; j += 2)
            {
                
                room->tiles[j][i].type = type;
            }
        }
    }
}

void putLshape(Room *room, Vec2i topLeft, Vec2i bottomRight, TileType type) 
{
    Vec2i bottomLeft;
    bottomLeft.x = topLeft.x;
    bottomLeft.y = bottomRight.y;

    patternBuilder(room, FILL, topLeft, bottomLeft, type);
    patternBuilder(room, FILL, bottomLeft, bottomRight, type);
}

void putUshape(Room *room, Vec2i topLeft, Vec2i bottomRight, TileType type) 
{
    Vec2i bottomLeft;
    bottomLeft.x = topLeft.x;
    bottomLeft.y = bottomRight.y;

    Vec2i topRight;
    topRight.x = bottomRight.x;
    topRight.y = topLeft.y;

    patternBuilder(room, FILL, topLeft, bottomLeft, type);
    patternBuilder(room, FILL, bottomLeft, bottomRight, type);
    patternBuilder(room, FILL, bottomRight, topRight, type);
}

void putTshape(Room *room, Vec2i topLeft, Vec2i bottomRight, TileType type) 
{
    Vec2i bottomLeft;
    bottomLeft.x = topLeft.x;
    bottomLeft.y = bottomRight.y;

    Vec2i topRight;
    topRight.x = bottomRight.x;
    topRight.y = topLeft.y;

    Vec2i topMid = Vec2i_middle(topLeft, topRight);
    Vec2i bottomMid = Vec2i_middle(bottomLeft, bottomRight);

    patternBuilder(room, FILL, topLeft, topRight, type);
    patternBuilder(room, FILL, topMid, bottomMid, type);
}

void presetStructures(Room *room, Structure structure, Vec2i topLeft, Vec2i bottomRight, TileType type, int angle)
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
            putLshape(room, topLeft, bottomRight, type);
            break;
        case U_SHAPE:
            putUshape(room, topLeft, bottomRight, type);
            break;
        case T_SHAPE:
            putTshape(room, topLeft, bottomRight, type);
            break;        
    }
}