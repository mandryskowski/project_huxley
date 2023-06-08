#ifndef ROOM_H
#define ROOM_H
typedef struct Entity Entity;
typedef unsigned int uint;
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
    Tile** tiles;
    uint width, height; // in tiles
    Entity** entities;

} Room;

Room Room_construct(uint width, uint height);
#endif // ROOM_H