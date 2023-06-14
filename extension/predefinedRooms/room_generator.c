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
    patternBuilder(room, CHECKERED, topLeft, bottomRight, TILE_WALL);
 
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

    //ADDING THE DOORS
    room->tiles[0][width / 2 - 1].type = TILE_DOOR;
    room->tiles[0][width / 2].type = TILE_DOOR;
    room->tiles[height / 2 - 1][0].type = TILE_DOOR;
    room->tiles[height / 2][0].type = TILE_DOOR;
}

Room *generate_room(int seed, Mode mode)
{
    Room *room = malloc(sizeof(Room));

    if (seed == -1)
    {
        srand(time(NULL));
    }
    else
    {
        srand(seed);
    }

    //SETTING SIZE
    int height = rand() % 5 * 2 + 20;
    int width = rand() % 5 * 2 + 20;
    printf("height is %d and width is %d\n", height, width);

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

int main()
{
    Room *room = generate_room(-1, INSANE);
    MonsterType** monsters = spawn_monsters(room, INSANE);
    room_to_file(room, monsters);
    printf("new room generated\n");
}
