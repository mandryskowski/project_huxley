#include "room.h"
#include <stdlib.h>
#include <stdio.h>
#include "game_math.h"
#include "state.h"
#include "entity.h"
#include "level.h"
#include "item.h"

TileType getTile(Vec2i vec, GameState *state)
{
    return state->currentLevel->currentRoom->tiles[vec.x][vec.y].type;
}

int tile_type_to_textureID(TileType tileType, RoomType roomType, int x, int y, Room *room)
{
    switch (tileType)
    {
        case TILE_FLOOR:
            return 12;
        case TILE_BARRIER:
            return roomType == ITEM_ROOM ? 11 : 15;
        case TILE_DOOR:
        {
            int non_zero = x && x != room->size.x - 1 ? x - room->size.x / 2 : y - room->size.y / 2;
            return non_zero ? 9 : 10;
        }
        case TILE_HOLE:
            return 8;
        case TILE_WALL:
            return 13;
        default:
            return 0;
    }
}

Room *construct_room(char *filename , RoomType type)
{
    FILE *file = fopen(filename, "r");
    int height, width;
    fscanf(file, "%d %d", &width, &height);

    Room *room = malloc(sizeof(Room));

    room->size = (Vec2i){width, height};
    room->entity_cnt = 1;
    room->tiles = malloc(width * sizeof(Tile*));
    room->visited = false;
    room->type = type;

    for (int x = 0; x < width; x++)
    {
        room->tiles[x] = malloc(height * sizeof(Tile));
        for (int y = 0; y < height; y++)
        {
            TileType tileType;
            fscanf(file, "%d", &tileType);
            room->tiles[x][y] = (Tile){.textureID = tile_type_to_textureID(tileType, type, x, y, room) , .type = tileType};
        }
    }

    room->entities = calloc(1000, sizeof(Entity *));

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            MonsterType monsterType;
            fscanf(file, "%d", &monsterType);
            if (monsterType != NOT_MONSTER)
            {
                room->entities[room->entity_cnt++] = construct_monster((Vec2d){x + 0.5, y + 0.5}, monsterType, room);
            }
        }
    }

    if (room->type == ITEM_ROOM)
    {
        room->entities[room->entity_cnt++] = construct_item(rand() % 10, (Vec2d){room->size.x / 2, room->size.y / 2});
    }

    if (room->type == BOSS_ROOM)
    {
        printf("%d entity cnt\n", room->entity_cnt);
    }

    fclose(file);
    return room;
}

bool isClear(Room *room)
{
    for (Entity **entity = room->entities; *entity; entity++)
    {
        if ((*entity)->faction == ENEMY && !isProjectile(*entity) && !isMine(*entity))
        {
            return false;
        }
    }

    return true;
}