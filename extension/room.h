#ifndef ROOM_H
#define ROOM_H

#include "math.h"
#include "state.h"

typedef struct Entity Entity;
typedef enum
{
    TILE_FLOOR,
    TILE_BARRIER,
    TILE_WALL,
    TILE_HOLE,

    TILE_FIRST = TILE_FLOOR,
    TILE_LAST = TILE_HOLE
} TileType;

typedef struct Tile
{
    TileType type;
    int textureID;
} Tile;

typedef struct Room
{
    Tile tiles[16][16];
    Entity** entities;
    
} Room;

TileType getTile(Vec2i, GameState *);

#endif // ROOM_H