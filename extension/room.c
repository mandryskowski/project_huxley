#include "room.h"
#include <stdlib.h>
#include <stdio.h>

Room Room_construct(uint width, uint height)
{
    Tile** tiles = malloc(width * sizeof(Tile*));
    for (int x = 0; x < width; x++)
    {
        tiles[x] = malloc(height * sizeof(Tile));
        for (int y = 0; y < height; y++)
        {
            tiles[x][y] = (Tile){.textureID = 0, .type = TILE_FLOOR};
        }
    }
    for (int x = 0; x < width; x++)
    {
        tiles[x][0] = (Tile){.textureID = 2, .type = TILE_WALL};
        tiles[x][height - 1] = (Tile){.textureID = 2, .type = TILE_WALL};
    }
    for (int y = 0; y < height; y++)
    {
        tiles[0][y] = (Tile){.textureID = 2, .type = TILE_WALL};
        tiles[width - 1][y] = (Tile){.textureID = 2, .type = TILE_WALL};
    }
    return (Room){.entities = NULL, .tiles = tiles, .width = width, .height = height};
}