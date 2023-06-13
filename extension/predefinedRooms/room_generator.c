#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "../room.h"
#include "../entity.h"
#include "../game_math.h"

void generate_room(int seed)
{
    if (seed == -1)
    {
        srand48(clock());
    }
    else
    {
        srand(seed);
    }

    int height = rand() % 5 * 2 + 20, width = rand() % 5 * 2 + 20;
    TileType tiles[height][width];
    int monsters[height][width];

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
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (tiles[i][j] != TILE_FLOOR)
            {
                monsters[i][j] = NOT_MONSTER;
                continue;
            }
            int prob = rand() % 300;
            if (prob < 1)
            {
                monsters[i][j] = ZOMBIE;
            }
            else if (prob < 0)
            {
                monsters[i][j] = SHOOTER;
            }
            else if (prob < 0)
            {
                monsters[i][j] = FLYING_SHOOTER;
            }
            else
            {
                monsters[i][j] = NOT_MONSTER;
            }
        }
    }
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
    fclose(file);
}

int main()
{
    generate_room(-1);
}