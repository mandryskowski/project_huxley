#include "state.h"
#include "game_math.h"
#include "game.h"
#include "room.h"
#include "level.h"
#include "util.h"
#include "entity.h"
#include "predefinedRooms/room_generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void jump_to_next_room(GameState *state)
{
    printf("roomChange!\n");
    Vec2i direction = {0, 0};
    Vec2d playerPos = state->player->entity->pos;
    if (playerPos.x < 2)
    {
        direction = (Vec2i){-1, 0};
    }
    if (playerPos.y < 2)
    {
        direction = (Vec2i){0, -1};
    }
    if (playerPos.x + 2 > state->currentLevel->currentRoom->size.x)
    {
        direction = (Vec2i){1, 0};
    }
    if (playerPos.y + 2 > state->currentLevel->currentRoom->size.y)
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
        Room *oldRoom = level->currentRoom;
        level->prevRoomCoords = level->currRoomCoords;
        level->currRoomCoords = newRoomCoords;
        level->prevRoom = level->currentRoom;
        for (Entity **entity = level->currentRoom->entities; *entity; entity++)
        {
            if (isMine(*entity) || isProjectile(*entity))
            {
                killEntity(*entity);
            }
        }
        erase_dead(level->currentRoom);
        *level->currentRoom->entities = NULL;
        level->currentRoom = newRoom;
        newRoom->visited = true;
        *newRoom->entities = state->player->entity;
        state->player->entity->room = newRoom;

        if (direction.x)
        {
            int y_diff = (newRoom->size.y - oldRoom->size.y) / 2;
            int x_change = direction.x < 0 ? newRoom->size.x - 1 : 1 - oldRoom->size.x;
            state->player->entity->pos.y += y_diff;
            state->player->entity->pos.x += x_change;
        }
        else
        {
            int x_diff = (newRoom->size.x - oldRoom->size.x) / 2;
            int y_change = direction.y < 0 ? newRoom->size.y - 1 : 1 - oldRoom->size.y;
            state->player->entity->pos.y += y_change;
            state->player->entity->pos.x += x_diff;
        }

        push(state->player->prev_positions, state->player->entity->pos);
        for (Entity **entity = newRoom->entities + 1; *entity; entity++)
        {
            (*entity)->cooldown_left = 180;
        }
    }
}

//Vec2i dirs[4] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}}; 9 10
void create_door(Room *room, int site)
{
    switch (site) {
        case 0:
            room->tiles[0][room->size.y / 2] = (Tile){TILE_DOOR, 9};
            room->tiles[0][room->size.y / 2 + 1] = (Tile){TILE_DOOR, 10};
            break;
        case 1:
            printf("%d %d xd\n", room->size.x, room->size.y);
            room->tiles[room->size.x / 2][0] = (Tile){TILE_DOOR, 10};
            room->tiles[room->size.x / 2 + 1][0] = (Tile){TILE_DOOR, 9};
            break;
        case 2:
            room->tiles[room->size.x - 1][room->size.y / 2] = (Tile){TILE_DOOR, 10};
            room->tiles[room->size.x - 1][room->size.y / 2 + 1] = (Tile){TILE_DOOR, 9};
            break;
        case 3:
            room->tiles[room->size.x / 2][room->size.y - 1] = (Tile){TILE_DOOR, 9};
            room->tiles[room->size.x / 2 + 1][room->size.y - 1] = (Tile){TILE_DOOR, 10};
            break;
        default:
            perror("Error in create_door xd");
            exit(0);
    }
}

Level *construct_level(Player *player, int room_number)
{
    srand(time(NULL));

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
    Vec2i *to_add_end = to_add + 1;
    *to_add = (Vec2i){map_width / 2, map_width / 2};
    visited[map_width / 2][map_width / 2] = true;

    for (int i = 0; i < room_number; i++)
    {
        shuffle(to_add, to_add_end - to_add, sizeof(Vec2i));
        if (i)
        {
            generate_room();
            level->map[to_add->x][to_add->y] = construct_room("predefinedRooms/new_room", NORMAL_ROOM);
        }
        for (int j = 0; j < 4; j++)
        {
            Vec2i new_coord = Vec2i_add(*to_add, dirs[j]);
            if (new_coord.x < 0 || new_coord.x == map_width || new_coord.y < 0 || new_coord.y == map_width
                || visited[new_coord.x][new_coord.y])
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
    level->prevRoomCoords = level->currRoomCoords = (Vec2i){map_width / 2, map_width / 2};
    *level->currentRoom->entities = player->entity;
    player->entity->room = level->currentRoom;
    level->currentRoom->visited = true;
    player->entity->pos = (Vec2d){10, 10};

    Vec2i boss_room = {0, 0};
    for (int i = 0; i < map_width; i++)
    {
        for (int j = 0; j < map_width; j++)
        {
            if (!level->map[i][j])
            {
                int neighbours = 0;
                for (int k = 0; k < 4; k++) {
                    Vec2i new_coord = Vec2i_add((Vec2i){i, j}, dirs[k]);
                    if (new_coord.x < 0 || new_coord.x == map_width || new_coord.y < 0 || new_coord.y == map_width ||
                        !level->map[new_coord.x][new_coord.y])
                    {
                        continue;
                    }
                    neighbours++;
                }
                if (neighbours == 1)
                {
                    boss_room = (Vec2i){i, j};
                }
            }
        }
    }

    level->map[boss_room.x][boss_room.y] = construct_room("predefinedRooms/haskell_room", BOSS_ROOM);

    for (int i = 0; i < map_width; i++)
    {
        for (int j = 0; j < map_width; j++)
        {
            if (level->map[i][j])
            {
                printf("%d %d room exiosts\n", i, j);
                for (int k = 0; k < 4; k++)
                {
                    Vec2i new_coord = Vec2i_add((Vec2i){i, j}, dirs[k]);
                    if (new_coord.x < 0 || new_coord.x == map_width || new_coord.y < 0 || new_coord.y == map_width ||
                        !level->map[new_coord.x][new_coord.y])
                    {
                        continue;
                    }
                    printf("%d %d (%d, %d)\n", i, j, dirs[k].x, dirs[k].y);
                    create_door(level->map[i][j], k);
                }
            }
        }
    }

    free(to_add_cpy);

    printf("\n");

    return level;
}
