#ifndef ROOM_H
#define ROOM_H

#include <stdio.h>
#include "game_math.h"

typedef struct Player Player;
typedef struct Entity Entity;
typedef struct GameState GameState;
typedef unsigned int uint;
typedef enum
{
    TILE_FLOOR,
    TILE_BARRIER,
    TILE_WALL,
    TILE_HOLE,
    TILE_DOOR,

    TILE_FIRST = TILE_FLOOR,
    TILE_LAST = TILE_DOOR
} TileType;

typedef struct Tile
{
    TileType type;
    int textureID;
} Tile;

typedef struct Room
{
    Tile** tiles;
    Vec2i size;
    Entity** entities;
    int entity_cnt;

} Room;

TileType getTile(Vec2i vec, GameState *state);
Room *Room_construct(uint width, uint height, FILE *file, Player *player);
#endif // ROOM_H