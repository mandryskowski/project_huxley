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
    if (playerPos.x < 1)
    {
        direction = (Vec2i){-1, 0};
    }
    if (playerPos.y < 1)
    {
        direction = (Vec2i){0, -1};
    }
    if (playerPos.x + 1 > state->currentLevel->currentRoom->size.x)
    {
        direction = (Vec2i){1, 0};
    }
    if (playerPos.y + 1 > state->currentLevel->currentRoom->size.y)
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

    if (state->currentLevel->currentRoom->type == BOSS_ROOM && !isClear(state->currentLevel->currentRoom))
    {
        playSound(SOUND_HASKELL_APPEARS);
    }
    if (state->currentLevel->currentRoom->type == SHOP_ROOM)
    {
        playMusic(MUSIC_SHOP, MUSIC_SHOP_LAST);
    }
    else if (state->currentLevel->currentRoom->type == BOSS_ROOM && !isClear(state->currentLevel->currentRoom))
    {
        playMusic(MUSIC_BOSS, MUSIC_BOSS_LAST);
    }
    else
    {
        playMusic(NO_SOUND, NO_SOUND);
    }
}

//Vec2i dirs[4] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}}; 9 10
void create_door(Room *room, int site)
{
    switch (site) {
        case 0:
            room->tiles[0][room->size.y / 2 - 1] = (Tile){TILE_DOOR, 9};
            room->tiles[0][room->size.y / 2] = (Tile){TILE_DOOR, 10};
            break;
        case 1:
            printf("%d %d xd\n", room->size.x, room->size.y);
            room->tiles[room->size.x / 2 - 1][0] = (Tile){TILE_DOOR, 10};
            room->tiles[room->size.x / 2][0] = (Tile){TILE_DOOR, 9};
            break;
        case 2:
            room->tiles[room->size.x - 1][room->size.y / 2 - 1] = (Tile){TILE_DOOR, 10};
            room->tiles[room->size.x - 1][room->size.y / 2] = (Tile){TILE_DOOR, 9};
            break;
        case 3:
            room->tiles[room->size.x / 2 - 1][room->size.y - 1] = (Tile){TILE_DOOR, 9};
            room->tiles[room->size.x / 2][room->size.y - 1] = (Tile){TILE_DOOR, 10};
            break;
        default:
            perror("Error in create_door xd");
            exit(0);
    }
}

void free_level(Level *level)
{
    for (int width = 0; width < level->size.x; width++)
    {
        for (int height = 0; height < level->size.y; height++)
        {
            if (level->map[width][height])
            {
                free_room(level->map[width][height]);
            }
        }
        free(level->map[width]);
    }

    free(level->map);
    free(level);
}

static const Vec2i dirs[4] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

bool add_special_room(RoomType type, Level *level)
{
    Vec2i *room_coords = calloc(sizeof(Vec2i), level->size.x * level->size.y + 1);
    int pos_cnt = 0;

    for (int i = 0; i < level->size.x; i++)
    {
        for (int j = 0; j < level->size.y; j++)
        {
            if (!level->map[i][j])
            {
                int neighbours = 0;
                for (int k = 0; k < 4; k++) {
                    Vec2i new_coord = Vec2i_add((Vec2i){i, j}, dirs[k]);
                    if (new_coord.x < 0 || new_coord.x == level->size.x || new_coord.y < 0 || new_coord.y == level->size.y ||
                        !level->map[new_coord.x][new_coord.y])
                    {
                        continue;
                    }
                    neighbours++;
                }
                if (neighbours == 1)
                {
                    room_coords[pos_cnt++] = (Vec2i){i, j};
                }
            }
        }
    }

    if (!pos_cnt)
    {
        free(room_coords);
        return false;
    }

    Vec2i coords_taken = room_coords[rand() % pos_cnt];

    if (type != BOSS_ROOM)
    {
        generate_room(type);
        level->map[coords_taken.x][coords_taken.y] = construct_room("predefinedRooms/new_room", type);
    }
    else
    {
        level->map[coords_taken.x][coords_taken.y] = construct_room("predefinedRooms/haskell_room", BOSS_ROOM);
    }

    free(room_coords);
    return true;
}

Level *construct_level(Player *player, int room_number)
{
    srand(time(NULL));

    while (true)
    {
        int map_width = sqrt(room_number) + 2;
        Level *level = calloc(sizeof(Level), 1);
        level->map = calloc(sizeof(Room **), map_width);
        level->size = (Vec2i){map_width, map_width};

        bool visited[level->size.x][level->size.y];
        for (int i = 0; i < level->size.x; i++)
        {
            level->map[i] = calloc(sizeof(Room *), level->size.y);
            for (int j = 0; j < level->size.y; j++)
            {
                visited[i][j] = false;
            }
        }

        Vec2i *to_add = calloc(sizeof(Vec2i), level->size.x * level->size.y);
        Vec2i *to_add_cpy = to_add;
        Vec2i *to_add_end = to_add + 1;
        *to_add = (Vec2i){level->size.x / 2, level->size.y / 2};
        visited[level->size.x / 2][level->size.y / 2] = true;

        for (int i = 0; i < room_number; i++)
        {
            shuffle(to_add, to_add_end - to_add, sizeof(Vec2i));
            if (i)
            {
                generate_room(NORMAL_ROOM);
                level->map[to_add->x][to_add->y] = construct_room("predefinedRooms/new_room", NORMAL_ROOM);
            }
            for (int j = 0; j < 4; j++)
            {
                Vec2i new_coord = Vec2i_add(*to_add, dirs[j]);
                if (new_coord.x < 0 || new_coord.x == level->size.x || new_coord.y < 0 || new_coord.y == level->size.y
                    || visited[new_coord.x][new_coord.y])
                {
                    continue;
                }
                visited[new_coord.x][new_coord.y] = true;
                *to_add_end++ = new_coord;
            }

            to_add++;
        }

        free(to_add_cpy);

        level->map[level->size.x / 2][level->size.y / 2] = construct_room("predefinedRooms/start_room", NORMAL_ROOM);
        level->currentRoom = level->map[level->size.x / 2][level->size.y / 2];
        level->prevRoom = level->currentRoom;
        level->prevRoomCoords = level->currRoomCoords = (Vec2i){level->size.x / 2, level->size.y / 2};
        *level->currentRoom->entities = player->entity;
        player->entity->room = level->currentRoom;
        level->currentRoom->visited = true;
        player->entity->pos = (Vec2d){2, 2};

        if (!add_special_room(ITEM_ROOM, level) || !add_special_room(SHOP_ROOM, level) || !add_special_room(BOSS_ROOM, level))
        {
            free_level(level);
            continue;
        }

        for (int i = 0; i < level->size.x; i++)
        {
            for (int j = 0; j < level->size.y; j++)
            {
                if (level->map[i][j])
                {
                    for (int k = 0; k < 4; k++)
                    {
                        Vec2i new_coord = Vec2i_add((Vec2i){i, j}, dirs[k]);
                        if (new_coord.x < 0 || new_coord.x == level->size.x || new_coord.y < 0 || new_coord.y == level->size.y ||
                            !level->map[new_coord.x][new_coord.y])
                        {
                            continue;
                        }
                        create_door(level->map[i][j], k);
                    }
                }
            }
        }

        return level;
    }
}
