#ifndef ASSETS_H
#define ASSETS_H
#include <stdbool.h>

typedef unsigned int uint;
typedef enum MonsterType MonsterType;
typedef enum RoomType RoomType;
typedef struct Vec2i Vec2i;
typedef struct Vec2d Vec2d;
typedef struct Vec4d Vec4d;
typedef struct Entity Entity;
/*
typedef struct EntityTextureInformation
{
    Vec2d shadowOffset;
    double shadowStrength;

    Vec2d textureOffset;
    Vec2d textureScale;
} EntityTextureInformation; */

uint loadTexture(char* filename);
uint loadAtlas(char* filename, int width, int height);

uint getMonsterTextureID(MonsterType type);
Vec4d getRoomMinimapColor(RoomType type, bool visited, Vec2i roomCoords);

//EntityTextureInformation getEntityTexInfo(Entity* ent);

#endif // ASSETS_H