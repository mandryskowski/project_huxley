#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "room.h"
#include "pathfind.h"

bool isOutOfBounds(Vec2i a, Room* room) {
    return !(0 <= a.x && a.x < room->size.x
          && 0 <= a.y && a.y < room->size.y);
}

bool isWalkable(Room* room, Vec2i tile) {
    return room->tiles[tile.x][tile.y].type == TILE_FLOOR;
}

const int dx[10] = {0, -1, -1, -1, 0, 1, 1, 1, 0, -1};
const int dy[10] = {1, 1, 0, -1, -1, -1, 0, 1, 1, 1};

Vec2d* path(Vec2d start, Entity** entities, GameState* gState) {

    Vec2i startTile = Vec2d_to_Vec2i(start);

    Vec2i* queue = calloc(gState->currentRoom->size.x * gState->currentRoom->size.y, sizeof(Vec2i));
    int qFront = 0, qEnd = 0;
    queue[0] = startTile;
    int** bfs = calloc(gState->currentRoom->size.x, sizeof(int*));
    for(int i = 0; i < gState->currentRoom->size.x; i++) {
        bfs[i] = calloc(gState->currentRoom->size.y, sizeof(int));
    }
    int MAX_SIZE = gState->currentRoom->size.x * gState->currentRoom->size.y;
    bfs[startTile.x][startTile.y] = MAX_SIZE;

    while(qFront <= qEnd) {
        Vec2i currTile = queue[qFront++];
        for(int dir = 0; dir < 8; dir += 2) { //NSWE
            Vec2i nextTile = Vec2i_add(currTile, (Vec2i){dx[dir], dy[dir]});
            if(!isOutOfBounds(nextTile, gState->currentRoom)
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
        if (isProjectile(*entity)) {
            entity++;
            continue;
        }
        sz++;
        Vec2i tile = Vec2d_to_Vec2i((*entity)->pos);
        bfs[tile.x][tile.y] -= 10;
        entity++;
    }

    Vec2d* velocity = calloc(sz + 1, sizeof(Vec2d));

    Entity** entityPos = entities;
    int index = 0;

    while(*entityPos != NULL) {


        if (isProjectile(*entityPos)) {
            entityPos++;
            continue;
        }

        (*entityPos)->attack_velocity = Vec2d_scale(Vec2d_normalize((Vec2d){start.x + 0.5 - (*entityPos)->pos.x, start.y + 0.5 - (*entityPos)->pos.y}), (*entityPos)->attack_SPD);
        handle_attack(*entityPos, NULL, SPAWN_PROJECTILE);
        Vec2i tile = Vec2d_to_Vec2i((*entityPos)->pos);
        int maxNext = 0;
        Vec2i nextPos = tile;


        for(int dir = 1; dir < 9; dir++) //NSEW + DIAG
        {
            Vec2i nextTile = Vec2i_add(tile, (Vec2i){dx[dir], dy[dir]});
            if(!isOutOfBounds(nextTile, gState->currentRoom)
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
        velocity[index++] = Vec2i_to_Vec2d(Vec2i_add(nextPos, Vec2i_scale(tile, -1)));
        entityPos++;
    }

    for(int i = 0; i < gState->currentRoom->size.x; i++) {
       free(bfs[i]);
    }
    free(bfs);
    free(queue);
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
    Vec2d playerPos = (Vec2d){12, 12};
    Entity entity = Entity_construct();
    Entity entity2 = Entity_construct();
    entity2.pos = (Vec2d){13, 14};
    gState->currentRoom->tiles[1][2].type = TILE_WALL;
    gState->currentRoom->tiles[2][1].type = TILE_WALL;
    Entity** entities = calloc(3, sizeof(Entity));
    entities[0] = &entity;
    entities[1] = &entity2;
    Vec2d* paths = path(playerPos, entities, gState);
    for(int i = 0; i < 3; i++) {
	printf("%f %f\n", paths[i].x, paths[i].y);
    }
}
*/
