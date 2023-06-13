#ifndef ROOM_H
#define ROOM_H

#include <stdio.h>
#include <stdbool.h>

typedef struct Player Player;
typedef struct Entity Entity;
typedef struct Vec2i Vec2i;
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

typedef enum
{
    NORMAL_ROOM, BOSS_ROOM, SHOP_ROOM, QUEST_ROOM, NOT_ROOM
} RoomType;

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
    int entity_cnt;
    RoomType type;
} Room;

TileType getTile(Vec2i vec, GameState *state);
Room *construct_room(char *filename, RoomType type);
bool isClear(Room *room);
#endif // ROOM_H