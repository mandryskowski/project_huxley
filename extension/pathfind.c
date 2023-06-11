#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "room.h"
#include "pathfind.h"

#define EPS 0.1f

typedef struct SortedEntity {
	Entity* entity;
	int index;
} SortedEntity;

bool isOutOfBounds(Vec2i a, Room* room) {
    return !(0 <= a.x && a.x < room->width
          && 0 <= a.y && a.y < room->height);
}

bool isWalkable(Room* room, Vec2i tile) {
    return room->tiles[tile.x][tile.y].type == TILE_FLOOR;
}

int bfsDist(Entity* entity, int** bfs) {
	return bfs[(int)floor(entity->pos.x)][(int)floor(entity->pos.y)];
}

Vec2d Vec2d_center(Rectangle rec)
{
    return Vec2d_scale(Vec2d_add(rec.bottomLeft, rec.topRight), 0.5f);
}

const int dx[10] = {0, -1, -1, -1, 0, 1, 1, 1, 0, -1};
const int dy[10] = {1, 1, 0, -1, -1, -1, 0, 1, 1, 1};

Vec2d* path(Vec2d start, Entity** entities, GameState* gState) {

    Vec2i startTile = Vec2d_to_Vec2i(start);

    Vec2i* queue = calloc(gState->currentRoom->width * gState->currentRoom->height, sizeof(Vec2i));
    int qFront = 0, qEnd = 0;
    queue[0] = startTile;
    int** bfs = calloc(gState->currentRoom->width, sizeof(int*));
    for(int i = 0; i < gState->currentRoom->width; i++) {
        bfs[i] = calloc(gState->currentRoom->height, sizeof(int));
    }
    int MAX_SIZE = gState->currentRoom->width * gState->currentRoom->height;
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


    bfs[startTile.x][startTile.y] = 2 * MAX_SIZE;

    // TRY TO INCENTIVISE GOING AROUND OTHER ENEMIES
    int sz = 0, nonProjIndex = 0;
    Entity** entity = entities;
    while(*entity != NULL) {
        if (isProjectile(*entity)) {
            entity++;
	    sz++;
            continue;
        }
        Vec2i tile = Vec2d_to_Vec2i((*entity)->pos);
        bfs[tile.x][tile.y] -= 10;
        entity++;
	sz++;
    }

    SortedEntity* sortedEntities = calloc(sz + 1, sizeof(SortedEntity));
    entity = entities;
    int ind2 = 0;
    while(*entity != NULL) {
	if(isProjectile(*entity)) {
		entity++; ind2++;
		continue;
	}
        sortedEntities[nonProjIndex++] = (SortedEntity){*entity, ind2};
/*        for(int ind = nonProjIndex - 1; ind > 0; ind--) {
                if(bfsDist(sortedEntities[ind].entity, bfs) > bfsDist(sortedEntities[ind-1].entity, bfs)) {
                        SortedEntity aux = sortedEntities[ind];
                        sortedEntities[ind] = sortedEntities[ind-1];
                        sortedEntities[ind - 1] = aux;
                }
                else
                        break;
        }
*/	entity++; ind2++;
    }



    Vec2d* velocity = calloc(sz + 1, sizeof(Vec2d));
    for(int i = 0; i < sz; i++)
	velocity[i] = (Vec2d){0.0, 0.0};

    SortedEntity* sEntity = sortedEntities;
    int index = 0;
    while(index < nonProjIndex) {
	Entity* entityPos = sEntity[index].entity;

	Vec2d hbCenter = Vec2d_add(Vec2d_center(entityPos->hitbox), entityPos->pos);

        Vec2i tile = Vec2d_to_Vec2i(entityPos->pos);
	Vec2d tileCenter = Vec2d_add(Vec2i_to_Vec2d(tile), (Vec2d){0.5f, 0.5f});

        int maxNext = 0;
        Vec2i nextPos = tile;
	int newDir = -1;

        for(int dir = 1; dir < 9; dir++) //NSEW + DIAG
        {
            Vec2i nextTile = Vec2i_add(tile, (Vec2i){dx[dir], dy[dir]});
            if(!isOutOfBounds(nextTile, gState->currentRoom)
            && isWalkable(gState->currentRoom, nextTile)
            && !(dir % 2 == 1
                && (!isWalkable(gState->currentRoom, Vec2i_add(tile, (Vec2i){dx[dir - 1], dy[dir - 1]}))
                || !isWalkable(gState->currentRoom, Vec2i_add(tile, (Vec2i){dx[dir + 1], dy[dir + 1]}))))
            )
            {
                if(maxNext < bfs[nextTile.x][nextTile.y]) {
                    maxNext = bfs[nextTile.x][nextTile.y];
                    nextPos = nextTile;
		    newDir = dir;
                }
            }
        }

	int vIndex = sEntity[index].index;

	if (newDir % 2 == 0
        && (!isWalkable(gState->currentRoom, Vec2i_add(tile, (Vec2i){dx[newDir - 1], dy[newDir - 1]}))
        || !isWalkable(gState->currentRoom, Vec2i_add(tile, (Vec2i){dx[newDir + 1], dy[newDir + 1]}))))
	{
		if((newDir % 4 == 2 && fabs(tileCenter.y - hbCenter.y) > EPS)
		 ||(newDir % 4 == 0 && fabs(tileCenter.x - hbCenter.x) > EPS)) {
	        	velocity[vIndex] = Vec2d_normalize(Vec2d_add(tileCenter, Vec2d_scale(hbCenter, -1.0f)));
		}
		else {
			bfs[nextPos.x][nextPos.y] -= 10;
		        velocity[vIndex] = Vec2d_normalize(Vec2i_to_Vec2d(Vec2i_add(nextPos, Vec2i_scale(tile, -1))));
		}
	}
	else
	{
		bfs[nextPos.x][nextPos.y] -= 10;
	        velocity[vIndex] = Vec2d_normalize(Vec2i_to_Vec2d(Vec2i_add(nextPos, Vec2i_scale(tile, -1))));
	}
	index++;
    }

    for(int i = 0; i < gState->currentRoom->width; i++) {
       free(bfs[i]);
    }
    free(bfs);
    free(queue);
    free(sortedEntities);
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
