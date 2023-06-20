#ifndef ASSETS_H
#define ASSETS_H
#include <stdbool.h>

typedef unsigned int uint;
typedef enum MonsterType MonsterType;
typedef enum RoomType RoomType;
typedef struct Vec2i Vec2i;
typedef struct Vec4d Vec4d;
uint loadTexture(char* filename);
uint loadAtlas(char* filename, int width, int height);

uint getMonsterTextureID(MonsterType type);
Vec4d getRoomMinimapColor(RoomType type, bool visited, Vec2i roomCoords);

#endif // ASSETS_H