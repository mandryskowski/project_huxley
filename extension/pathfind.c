#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "room.h"
#include "pathfind.h"

#define EPS 1e-6
#define MAX_SIZE 2048

bool isOutOfBounds(Vec2i a) {
    return !(0 <= a.x && a.x < 16
          && 0 <= a.y && a.y < 16);
}

bool isWalkable(Room* room, Vec2i tile) {
    return room->tiles[tile.x][tile.y].type == TILE_FLOOR;
}

const int dx[10] = {0, -1, -1, -1, 0, 1, 1, 1, 0, -1};
const int dy[10] = {1, 1, 0, -1, -1, -1, 0, 1, 1, 1};

Vec2f* path(Vec2f start, Entity** entities, GameState* gState) {

    Vec2i startTile = Vec2f_to_Vec2i(start);

    Vec2i queue[MAX_SIZE];
    int qFront = 0, qEnd = 0;
    queue[0] = startTile;
    int bfs[16][16];
    for(int i = 0; i < 16; i++)
        for(int j = 0; j < 16; j++)
            bfs[i][j] = 0;
    bfs[startTile.x][startTile.y] = MAX_SIZE;

    while(qFront <= qEnd) {
        Vec2i currTile = queue[qFront++];
        for(int dir = 0; dir < 8; dir += 2) { //NSWE
            Vec2i nextTile = Vec2i_add(currTile, (Vec2i){dx[dir], dy[dir]});
            if(!isOutOfBounds(nextTile)
            && isWalkable(gState->currentRoom, nextTile)
            && bfs[nextTile.x][nextTile.y] == 0)
            {
                bfs[nextTile.x][nextTile.y] = bfs[currTile.x][currTile.y] - 1;
                queue[++qEnd] = nextTile;
            }
        }
    }

    // TRY TO INCENTIVISE GOING AROUND OTHER ENEMIES
    int sz = 0;
    Entity** entity = entities;
    while(*entity != NULL) {
        sz++;
        Vec2i tile = Vec2f_to_Vec2i((*entity)->pos);
        bfs[tile.x][tile.y]-=2;
        entity++;
    }

    Vec2f* velocity = calloc(sz + 1, sizeof(Vec2f));

    Entity** entityPos = entities;
    int index = 0;
    while(*entityPos != NULL) {
        Vec2i tile = Vec2f_to_Vec2i((*entityPos)->pos);
        int maxNext = 0;
        Vec2i nextPos = tile;

        for(int dir = 1; dir < 9; dir++) //NSEW + DIAG
        {
            Vec2i nextTile = Vec2i_add(tile, (Vec2i){dx[dir], dy[dir]});
            if(!isOutOfBounds(nextTile)
            && isWalkable(gState->currentRoom, nextTile)
            && !(dir % 2 == 1
                && !isWalkable(gState->currentRoom, Vec2i_add(tile, (Vec2i){dx[dir - 1], dy[dir - 1]}))
                && !isWalkable(gState->currentRoom, Vec2i_add(tile, (Vec2i){dx[dir + 1], dy[dir + 1]})))
            )
            {
                if(maxNext < bfs[nextTile.x][nextTile.y]) {
                    maxNext = bfs[nextTile.x][nextTile.y];
                    nextPos = nextTile;
                }
            }
        }
	printf("%d %d -> %d %d\n", tile.x, tile.y, nextPos.x, nextPos.y);
        velocity[index++] = Vec2i_to_Vec2f(Vec2i_add(nextPos, Vec2i_scale(tile, -1)));
        entityPos++;
    }

    return velocity;
}

//////////////
//Test cases//
//////////////
/*
int main()
{
    GameState* gState = calloc(1, sizeof(GameState));
    gState->currentRoom = calloc(1, sizeof(Room));
    Vec2f playerPos = (Vec2f){12, 12};
    Entity entity = Entity_construct();
    Entity entity2 = Entity_construct();
    entity2.pos = (Vec2f){13, 14};
    gState->currentRoom->tiles[1][2].type = TILE_WALL;
    gState->currentRoom->tiles[2][1].type = TILE_WALL;
    Entity** entities = calloc(3, sizeof(Entity));
    entities[0] = &entity;
    entities[1] = &entity2;
    Vec2f* paths = path(playerPos, entities, gState);
    for(int i = 0; i < 3; i++) {
	printf("%f %f\n", paths[i].x, paths[i].y);
    }
}
*/
