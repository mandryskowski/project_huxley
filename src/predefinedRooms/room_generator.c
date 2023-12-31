#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "../room.h"
#include "../entity.h"
#include "../movement.h"
#include "../game_math.h"
#include "structure_builder.h"
#include "generator_attributes.h"
#include "../haskell.h"
#include "monster_spawner.h"

void put_tiles(Room* room, Mode mode)
{
    int height = room->size.y;
    int width = room->size.x;

    // for (int i = 1; i < height - 1; i++)
    // {
    //     for (int j = 1; j < width - 1; j++)
    //     {
    //         int prob = rand() % 100;
    //         if (prob < 93)
    //         {
    //             tiles[i][j] = TILE_FLOOR;
    //         }
    //         else if (prob < 94)
    //         {
    //             tiles[i][j] = TILE_HOLE;
    //         }
    //         else if (prob < 97)
    //         {
    //             tiles[i][j] = TILE_BARRIER;
    //         }
    //         else
    //         {
    //             tiles[i][j] = TILE_WALL;
    //         }
    //     }
    // }

    //VISIBLE TILES 
    Vec2i topLeft = (Vec2i) {5, height - 5};
    Vec2i topRight = (Vec2i) {width - 2, height - 2};
    Vec2i bottomRight = (Vec2i) {width - 5, 5};
    Vec2i bottomLeft = (Vec2i) {2, 2};
    Vec2i middle = Vec2i_middle(topLeft, bottomRight);

    //SEGMENTATION FAULT MEANS THERE IS NOT ENOUGH ROOM TO SPAWN THE PLAYER
    int pattern_num = rand() % 4 + 1;
    //int pattern_num = 1;
    for (int i = 0; i < pattern_num; i++)
    {
        for (int j = 0; j < pattern_num; j++)
        {
            int res = rand() % 4 + 1;
            int type = rand() % 3 + 1;
            int pattern_height = (height - 5) / pattern_num - 1;
            int pattern_width = (width - 5) / pattern_num - 1;
            // 3->pattern_num, 18 18, 4, 4; {3, 6}{6 ,3} {8, }
            Vec2i temp_top_left = {j * (pattern_width + 1) + 3, (i + 1) * (pattern_height + 1) + 1};
            Vec2i temp_bottom_right = {(j + 1) * (pattern_width + 1) + 1, i * (pattern_height + 1) + 3};
            //printf("%d %d\n", res, type);
            patternBuilder(room, res, temp_top_left, temp_bottom_right, type);
        }
    }
 
    //ADDING THE OUTLINE
    for (int i = 0; i < height; i++)
    {
        room->tiles[i][0].type = TILE_WALL;
        room->tiles[i][width - 1].type = TILE_WALL;
    }
    for (int i = 0; i < width; i++)
    {
        room->tiles[0][i].type = TILE_WALL;
        room->tiles[height - 1][i].type = TILE_WALL;
    }

}

void set_pedestal(Room *room, Vec2i top_left)
{
    room->tiles[top_left.y][top_left.x].type = TILE_BARRIER;
    room->tiles[top_left.y][top_left.x + 1].type = TILE_BARRIER;
    room->tiles[top_left.y + 1][top_left.x].type = TILE_BARRIER;
    room->tiles[top_left.y + 1][top_left.x + 1].type = TILE_BARRIER;
}

Room *generate_empty_item_room(RoomType type)
{
    Room *room = malloc(sizeof(Room));
    room->type = type;

    int height = 10;
    int width = type == ITEM_ROOM ? 10 : 20;

    Tile** tiles = calloc(height, sizeof(Tile*));
    MonsterType **monsters = calloc(height, sizeof(MonsterType *));
    for(int i=0;i<height;i++)
    {
        tiles[i] = calloc(width, sizeof(Tile));
        if(tiles[i] == NULL) {exit(EXIT_FAILURE);}
    }
    if(tiles == NULL) {exit(EXIT_FAILURE);}

    room->tiles = tiles;
    room->size.y = height;
    room->size.x = width;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            room->tiles[i][0].type = TILE_FLOOR;
            room->tiles[i][width - 1].type = TILE_FLOOR;
        }
    }

    for (int i = 0; i < height; i++)
    {
        room->tiles[i][0].type = TILE_WALL;
        room->tiles[i][width - 1].type = TILE_WALL;
    }
    for (int i = 0; i < width; i++)
    {
        room->tiles[0][i].type = TILE_WALL;
        room->tiles[height - 1][i].type = TILE_WALL;
    }

    if (type == ITEM_ROOM)
    {
        set_pedestal(room ,(Vec2i){room->size.x / 2 - 1, room->size.y / 2 - 1});
    }

    else
    {
        set_pedestal(room ,(Vec2i){room->size.x / 2 - 1 - 5, room->size.y / 2 - 1});
        set_pedestal(room ,(Vec2i){room->size.x / 2 - 1, room->size.y / 2 - 1});
        set_pedestal(room ,(Vec2i){room->size.x / 2 - 1 + 5, room->size.y / 2 - 1});
    }

    return room;
}

Room *generate_empty_room(Mode mode, RoomType type)
{
    if (type == ITEM_ROOM || type == SHOP_ROOM)
    {
        return generate_empty_item_room(type);
    }

    Room *room = malloc(sizeof(Room));

    //SETTING SIZE
    int height = rand() % 5 * 2 + 20;
    int width = rand() % 5 * 2 + 20;

    //Allocating tile mem.
    Tile** tiles = calloc(height, sizeof(Tile*));
    MonsterType **monsters = calloc(height, sizeof(MonsterType *));
    for(int i=0;i<height;i++)
    {
        tiles[i] = calloc(width, sizeof(Tile));
        if(tiles[i] == NULL) {exit(EXIT_FAILURE);}
    }
    if(tiles == NULL) {exit(EXIT_FAILURE);}

    room->tiles = tiles;
    room->size.y = height;
    room->size.x = width;

    put_tiles(room, mode);

    return room;
}

void room_to_file(Room* room, MonsterType** monsters)
{
    int width = room->size.x;
    int height = room->size.y; 

    FILE *file = fopen("predefinedRooms/new_room", "w");
    fprintf(file, "%d %d\n", height, width);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j)
            {
                fprintf(file, " ");
            }
            fprintf(file, "%d", room->tiles[i][j].type);
        }
        fprintf(file, "\n");
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j)
            {
                fprintf(file, " ");
            }
            fprintf(file, "%d", monsters[i][j]);
        }
        fprintf(file, "\n");
    }

    //Tile** tiles = room->tiles;
    //Freeing tile space.
    for(int i = 0; i < height; i++)
    {
        free(room->tiles[i]);
    }
    free(room->tiles);

    //Freeing monsters space.
    for(int i = 0; i < height; i++)
    {
        free(monsters[i]);
    }
    free(monsters);

    free(room);

    fclose(file);
}

void generate_room(int type)
{
    Room *room = generate_empty_room(INSANE, type);
    MonsterType** monsters = spawn_monsters(room, INSANE, type);
    room_to_file(room, monsters);
    printf("new room generated\n");
}
