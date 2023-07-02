#ifndef ROOM_H
#define ROOM_H

#include <stdio.h>
#include "game_math.h"
#include <stdbool.h>

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

typedef enum RoomType
{
    NORMAL_ROOM, BOSS_ROOM, SHOP_ROOM, QUEST_ROOM, ITEM_ROOM, NOT_ROOM
} RoomType;

typedef void (*effect)(Player *);

typedef struct TileEffect
{
    effect eff;
    int duration;
} TileEffect;

typedef struct Tile
{
    TileType type;
    int textureID;
    TileEffect tile_effect;
} Tile;

typedef struct Room
{
    Tile** tiles;
    Vec2i size;
    Entity** entities;
    int entity_cnt;
    RoomType type;
    bool visited;
} Room;

TileType getTile(Vec2i vec, Room *room);
void add_items_to_shop(Room *room);
Room *construct_room(char *filename, RoomType type);
bool isClear(Room *room);
void free_room(Room *room);

void frozen(Player *player);
void on_fire(Player *player);
#endif // ROOM_H