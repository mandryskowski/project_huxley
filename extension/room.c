#include "room.h"
#include <stdlib.h>
#include <stdio.h>
#include "game_math.h"
#include "state.h"
#include "entity.h"

TileType getTile(Vec2i vec, GameState *state)
{
    return state->currentRoom->tiles[vec.x][vec.y].type;
}

Room *Room_construct(uint width, uint height, FILE *file, Player *player)
{
    Room *room = malloc(sizeof(Room));

    room->width = width;
    room->height = height;
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

    for (int x = 1; x < width - 1; x++)
    {
        for (int y = 1; y < height - 1; y++)
        {
            bool clear = true;
            for (int k = -1; k < 2; k++)
            {
                for (int l = -1; l < 2; l++)
                {
                    if (room->tiles[x + k][y + l].type != TILE_FLOOR)
                    {
                        clear = false;
                    }
                }
            }
            if (clear)
            {
                player->entity->pos = (Vec2d){x + 0.5, y + 0.5};
                player->entity->room = room;
                *room->entities = player->entity;
            }
        }
    }


    return room;
}