#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "room.h"
#include "math.h"
#include "entity.h"

#define EPS 1e-6
#define MAX_SIZE 2048

Vec2i getTile(Vec2f pos) {
    return (Vec2i){floor(pos.x), floor(pos.y)};
}

bool Vec2i_equals(Vec2i a, Vec2i b) {
    return (a.x == b.x && a.y == b.y);
}

bool isOutOfBounds(Vec2i a) {
    return !(0 <= a.x && a.x < 16
          && 0 <= a.y && a.y < 16);
}

Vec2i Vec2iSum(Vec2i a, Vec2i b) {
    return (Vec2i){a.x + b.x, a.y + b.y};
}

Vec2i newVec2i(int a, int b) {
    return (Vec2i){a, b};
}

bool isWalkable(State* state, Vec2i tile) {
    return state->tiles[tile.x][tile.y].type == TILE_FLOOR;
}


const int dx[10] = {0, -1, -1, -1, 0, 1, 1, 1, 0, -1};
const int dy[10] = {1, 1, 0, -1, -1, -1, 0, 1, 1, 1};

Vec2f* path(Vec2f start, Vec2f** end, const State* state) {

    Vec2i startTile = getTile(start);

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
            Vec2i nextTile = Vec2iSum(currTile, newVec2i(dx[dir], dy[dir]));
            if(!isOutOfBounds(nextTile) 
            && isWalkable(state, nextTile)
            && !(dir % 2 == 0
                && !isWalkable(state, Vec2iSum(currTile, newVec2i(dx[dir - 1], dy[dir - 1])))
                && !isWalkable(state, Vec2iSum(currTile, newVec2i(dx[dir + 1], dy[dir + 1]))))
            && bfs[nextTile.x][nextTile.y] == 0) 
            {
                bfs[nextTile.x][nextTile.y] = bfs[currTile.x][currTile.y] - 1;
                queue[++qEnd] = nextTile;
            }
        }
    }

    printf("1\n");

    // TRY TO INCENTIVISE GOING AROUND OTHER ENEMIES
    int sz = 0;
    Vec2f** entity = end;
    while(*entity != NULL) {
        printf("2\n");
        sz++;
        Vec2i tile = getTile(**entity);
        printf("%d %d\n", tile.x, tile.y);
        bfs[tile.x][tile.y]--;
        *entity++;
    }
    printf("4\n");

    Vec2f* velocity = calloc(sz + 1, sizeof(Vec2f));

    printf("3\n");

    Vec2f** entityPos = end;
    int index = 0;
    while(*entityPos != NULL) {
        Vec2i tile = getTile(**entityPos);
        int maxNext = 0;
        Vec2i nextPos = tile;

        for(int dir = 1; dir < 9; dir++) //NSEW + DIAG
        {
            Vec2i nextTile = Vec2iSum(tile, newVec2i(dx[dir], dy[dir]));
            if(!isOutOfBounds(nextTile)
            && isWalkable(state, nextTile)
            && !(dir % 2 == 1
                && !isWalkable(state, Vec2iSum(tile, newVec2i(dx[dir - 1], dy[dir - 1])))
                && !isWalkable(state, Vec2iSum(tile, newVec2i(dx[dir + 1], dy[dir + 1]))))
            )
            {
                if(maxNext < bfs[nextTile.x][nextTile.y]) {
                    maxNext = bfs[nextTile.x][nextTile.y];
                    nextPos = nextTile;
                }
            }
        }

        velocity[index++] = (Vec2f){(float)(nextPos.x - tile.x), (float)(nextPos.y - tile.y)};
        *entityPos++;
    }

    return velocity;
}


