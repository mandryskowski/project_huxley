#include "state.h"
#include "game_math.h"
#include "room.h"
#include "level.h"
#include "util.h"
#include "entity.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void jump_to_next_room(GameState *state)
{
    Vec2i direction = {0, 0};
    Vec2d playerPos = state->player->entity->pos;
    if (playerPos.x < 1)
    {
        direction = (Vec2i){-1, 0};
    }
    if (playerPos.y < 1)
    {
        direction = (Vec2i){0, -1};
    }
    if (playerPos.x + 1 > state->currentLevel->currentRoom->width)
    {
        direction = (Vec2i){1, 0};
    }
    if (playerPos.y + 1 > state->currentLevel->currentRoom->height)
    {
        direction = (Vec2i){0, 1};
    }

    Level *level = state->currentLevel;
    Vec2i newRoomCoords = Vec2i_add(level->currRoomCoords, direction);

    //room outside of map.
    if (!level->map[newRoomCoords.x] || !(level->map[newRoomCoords.x] + newRoomCoords.y))
    {
        return;
    }

    Room *newRoom = level->map[newRoomCoords.x][newRoomCoords.y];

    if (isClear(level->currentRoom) && newRoom->type != NOT_ROOM)
    {
        level->currRoomCoords = newRoomCoords;
        *level->currentRoom->entities = NULL;
        *newRoom->entities = state->player->entity;
        level->currentRoom = newRoom;
        state->player->entity->pos = (Vec2d){level->currentRoom->height * (1 + direction.x) / 2,
                                             level->currentRoom->width * (1 + direction.y) / 2};
        state->renderNewRoom = true;
    }
}

//Vec2i dirs[4] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}}; 9 10
void create_door(Room *room, int site)
{
    switch (site) {
        case 0:
            room->tiles[room->height / 2][0] = (Tile){TILE_DOOR, 9};
            room->tiles[room->height / 2 + 1][0] = (Tile){TILE_DOOR, 10};
            break;
        case 1:
            room->tiles[0][room->width / 2] = (Tile){TILE_DOOR, 10};
            room->tiles[0][room->width / 2 + 1] = (Tile){TILE_DOOR, 9};
        case 2:
            room->tiles[room->height / 2][room->width - 1] = (Tile){TILE_DOOR, 10};
            room->tiles[room->height / 2 + 1][room->width - 1] = (Tile){TILE_DOOR, 9};
            break;
        case 3:
            room->tiles[room->height - 1][room->width / 2] = (Tile){TILE_DOOR, 9};
            room->tiles[room->height - 1][room->width / 2 + 1] = (Tile){TILE_DOOR, 10};
        default:
            perror("Error in create_door");
            exit(0);
    }
}

Level *construct_level(Player *player, int room_number)
{
    Level *level = calloc(sizeof(Level), 1);
    int map_width = sqrt(room_number) + 2;

    bool visited[map_width][map_width];
    level->map = calloc(sizeof(Room **), map_width);


    for (int i = 0; i < map_width; i++)
    {
        level->map[i] = calloc(sizeof(Room *), map_width);
        for (int j = 0; j < map_width; j++)
        {
            visited[i][j] = false;
        }
    }

    Vec2i dirs[4] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
    Vec2i *to_add = calloc(sizeof(Vec2i), map_width * map_width);
    Vec2i *to_add_cpy = to_add;
    Vec2i *to_add_end = to_add;
    *to_add = (Vec2i){map_width / 2, map_width / 2};
    visited[map_width / 2][map_width / 2] = true;

    for (int i = 0; i < room_number; i++)
    {
        shuffle(to_add, to_add_end - to_add, sizeof(Vec2i));
        if (i)
        {
            system("./predefinedRooms/room_generator");
            level->map[to_add->x][to_add->y] = construct_room("predefinedRooms/new_room", NORMAL_ROOM);
        }
        for (int j = 0; j < 4; j++)
        {
            Vec2i new_coord = Vec2i_add(*to_add, dirs[j]);
            if (new_coord.x < 0 || new_coord.x == map_width || new_coord.y < 0 || new_coord.y == map_width)
            {
                continue;
            }
            visited[new_coord.x][new_coord.y] = true;
            *to_add_end++ = new_coord;
        }

        to_add++;
    }

    level->map[map_width / 2][map_width / 2] = construct_room("predefinedRooms/start_room", NORMAL_ROOM);
    level->currentRoom = level->map[map_width / 2][map_width / 2];
    level->currRoomCoords = (Vec2i){map_width / 2, map_width / 2};
    *level->currentRoom->entities = player->entity;
    player->entity->room = level->currentRoom;
    player->entity->pos = (Vec2d){10, 10};

    for (int i = 0; i < map_width; i++)
    {
        for (int j = 0; j < map_width; j++)
        {
            if (level->map[i][j])
            {
                for (int k = 0; k < 4; k++)
                {
                    Vec2i new_coord = Vec2i_add(*to_add, dirs[k]);
                    if (new_coord.x < 0 || new_coord.x == map_width || new_coord.y < 0 || new_coord.y == map_width)
                    {
                        continue;
                    }
                    create_door(level->map[i][j], k);
                }
            }
        }
    }

    free(to_add_cpy);

    return level;
}
