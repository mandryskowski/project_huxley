#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "../room.h"
#include "../entity.h"
#include "../game_math.h"

typedef enum {EASY, MEDIUM, HARD, INSANE} Mode;
typedef enum {Horizontal_LINE,Vertical_LINE,L_SHAPE, U_SHAPE, T_SHAPE} Structure;
#define AVG(a,b) ((a+b)/2.0)

double position_chance(Vec2i current_i, int height, int width, Mode mode)
{
    Vec2d current = Vec2i_to_Vec2d(current_i);

    Vec2d middle;
    middle.x = ((double)(width)) / 2.0;
    middle.y = ((double)(height)) / 2.0;

    //Distance from the doors
    double leftDistance = Vec2d_metric_distance(current,(Vec2d) {0, middle.y});
    double rightDistance = Vec2d_metric_distance(current, (Vec2d) {2.0 * middle.x , middle.y});

    //Closest door to the mob
    double distance = min(leftDistance, rightDistance);

    //20% higher chance of mobs spawning every time the mode becomes more intense
    double modifier = ((double) mode) * (0.2) + 1;

    return modifier * (distance / middle.x);

}


void put_monsters(MonsterType** monsters, TileType** tiles, int height, int width, Mode mode)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (tiles[i][j] != TILE_FLOOR)
            {
                monsters[i][j] = NOT_MONSTER;
                continue;
            }

            double prob = AVG(((double) (rand() % 1000)) ,
                              1000 * position_chance((Vec2i) {j, i} ,height, width, mode));
            double threshold = ((double) mode) * 50;

            if (prob < 100 + threshold)
            {
                monsters[i][j] = ZOMBIE;
            }
            if (prob < 80 + threshold)
            {
                monsters[i][j] = SHOOTER;
            }
            if (prob < 20 + threshold)
            {
                monsters[i][j] = FLYING_SHOOTER;
            }
            else
            {
             monsters[i][j] = NOT_MONSTER;
            }
        }
    }
}


void put_tiles(TileType** tiles,int height, int width, Mode mode)
{
    for (int i = 0; i < height; i++)
    {
        tiles[i][0] = TILE_WALL;
        tiles[i][width - 1] = TILE_WALL;
    }
    for (int i = 0; i < width; i++)
    {
        tiles[0][i] = TILE_WALL;
        tiles[height - 1][i] = TILE_WALL;
    }

    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            int prob = rand() % 100;
            if (prob < 93)
            {
                tiles[i][j] = TILE_FLOOR;
            }
            else if (prob < 94)
            {
                tiles[i][j] = TILE_HOLE;
            }
            else if (prob < 97)
            {
                tiles[i][j] = TILE_BARRIER;
            }
            else
            {
                tiles[i][j] = TILE_WALL;
            }
        }
    }

    tiles[0][width / 2 - 1] = TILE_DOOR;
    tiles[0][width / 2] = TILE_DOOR;
    tiles[height / 2 - 1][0] = TILE_DOOR;
    tiles[height / 2][0] = TILE_DOOR;
}

void generate_room(int seed)
{
    //Needs implementation as parameter.
    Mode default_mode = EASY;

    if (seed == -1)
    {
        srand(time(0));
    }
    else
    {
        srand(seed);
    }

    int height = rand() % 5 * 2 + 20;
    int width = rand() % 5 * 2 + 20;
    printf("height is %d and width is %d\n", height, width);

    //Allocating tile mem.
    TileType** tiles = calloc(height, sizeof(TileType*));
    for(int i=0;i<height;i++)
    {
        tiles[i] = calloc(width, sizeof(TileType));
        if(tiles[i] == NULL) {exit(EXIT_FAILURE);}
    }
    if(tiles == NULL) {exit(EXIT_FAILURE);}

    //Allocating monsters mem.
    MonsterType** monsters = calloc(height, sizeof(MonsterType*));
    for(int i = 0; i < height; i++)
    {
        monsters[i] = calloc(width, sizeof(MonsterType));
        if(monsters[i] == NULL) {exit(EXIT_FAILURE);}
    }
    if(monsters == NULL) {exit(EXIT_FAILURE);}

    //Putting down the elements.
    put_tiles(tiles, height, width, default_mode);
    put_monsters(monsters, tiles, height, width, default_mode);

    //Generating output file.
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
            fprintf(file, "%d", tiles[i][j]);
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

    // Freeing tile space.
    for(int i = 0; i < height; i++)
    {
        free(tiles[i]);
    }
    free(tiles);

    //Freeing monsters space.
    for(int i = 0; i < height; i++)
    {
        free(monsters[i]);
    }
    free(monsters);

    fclose(file);
}

//

int main()
{
    generate_room(-1);
    printf("new room generated\n");
}