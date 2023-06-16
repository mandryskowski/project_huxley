#include "room.h"
#include <stdlib.h>
#include <stdio.h>
#include "game_math.h"
#include "state.h"
#include "entity.h"
#include "level.h"

TileType getTile(Vec2i vec, GameState *state)
{
    return state->currentLevel->currentRoom->tiles[vec.x][vec.y].type;
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

    for (int x = 0; x < width; x++)
    {
        room->tiles[x] = malloc(height * sizeof(Tile));
        for (int y = 0; y < height; y++)
        {
            TileType type;
            fscanf(file, "%d", &type);
            room->tiles[x][y] = (Tile){.textureID = 0, .type = type};
        }
    }

    room->entities = calloc(1000, sizeof(Entity *));

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            MonsterType type;
            fscanf(file, "%d", &type);
            if (type != NOT_MONSTER)
            {
                room->entities[room->entity_cnt++] = construct_monster((Vec2d){x + 0.5, y + 0.5}, type, room);
            }
        }
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