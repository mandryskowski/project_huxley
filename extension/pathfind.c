#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "pqueue.h"
#include "room.h"
#include "pathfind.h"
#include "level.h"
#include "util.h"

#define EPS 1e-5
#define D_INF 10000000.0
typedef struct SortedEntity {
	Entity* entity;
	int index;
} SortedEntity;

typedef struct PQElement {
	double value;
	Vec2i tile;
} PQElement;

PQElement *newPQElement(double value, Vec2i tile) {
    PQElement* newEl = calloc(1, sizeof(PQElement));
    newEl->value = value;
    newEl->tile = tile;
    return newEl;
}

bool isWalkable(Room* room, Vec2i tile) {
    return !isOutOfBounds(tile, room) && room->tiles[tile.x][tile.y].type == TILE_FLOOR;
}

int bfsDist(Entity* entity, double** bfs) {
	return bfs[(int)floor(entity->pos.x)][(int)floor(entity->pos.y)];
}

Vec2d Vec2d_center(Rectangle rec)
{
    return Vec2d_scale(Vec2d_add(rec.bottomLeft, rec.topRight), 0.5);
}

const int dx[10] = {0, -1, -1, -1, 0, 1, 1, 1, 0, -1};
const int dy[10] = {1, 1, 0, -1, -1, -1, 0, 1, 1, 1};

int _cmp(const void *_this, const void *_other) {
    return ((PQElement*)_this)->value > ((PQElement*)_other)->value;
}

Vec2d* path(Vec2d start, Entity** entities, GameState* gState) {

    Vec2i startTile = Vec2d_to_Vec2i(start);
    int MAX_SIZE = gState->currentLevel->currentRoom->size.x * gState->currentLevel->currentRoom->size.y;

    double** bfs = calloc(gState->currentLevel->currentRoom->size.x, sizeof(double*));
    for(int i = 0; i < gState->currentLevel->currentRoom->size.x; i++) {
        bfs[i] = calloc(gState->currentLevel->currentRoom->size.y, sizeof(double));
        for(int j = 0; j < gState->currentLevel->currentRoom->size.y; j++) {
            bfs[i][j] = D_INF;
        }
    }

    bfs[startTile.x][startTile.y] = 20000.0;

    PQueue* pq = pqueue_new(_cmp, MAX_SIZE);
    pqueue_enqueue(pq, newPQElement(20000.0, startTile));
 // Zombie dijkstra
    while(pq->size) {
        PQElement* pqtop = (PQElement*)pqueue_dequeue(pq);
	    Vec2i currTile = pqtop->tile;
        if (bfs[currTile.x][currTile.y] < pqtop->value)
        {
            continue;
        }
        for(int dir = 1; dir < 9; dir += 1) { //NSWE
            Vec2i nextTile = Vec2i_add(currTile, (Vec2i){dx[dir], dy[dir]});
            if(isWalkable(gState->currentLevel->currentRoom, nextTile)
                && !(dir % 2 == 1
                    && (!isWalkable(gState->currentLevel->currentRoom, Vec2i_add(currTile, (Vec2i){dx[dir - 1], dy[dir - 1]}))
                    || !isWalkable(gState->currentLevel->currentRoom, Vec2i_add(currTile, (Vec2i){dx[dir + 1], dy[dir + 1]}))))
                )
            {
                double dValue = ((dir%2) ? 1.4 : 1.0) + bfs[currTile.x][currTile.y];
                if(dValue < bfs[nextTile.x][nextTile.y]) {
                    bfs[nextTile.x][nextTile.y] = dValue;
                    pqueue_enqueue(pq, newPQElement(dValue, nextTile));
                }
            }
        }
    }
    bfs[startTile.x][startTile.y] = 0.0;

    // TRY TO INCENTIVISE GOING AROUND OTHER ENEMIES
    int sz = 0, nonProjIndex = 0;
    Entity** entity = entities;
    while(*entity != NULL) {
        if (isNotAMonster(*entity)) {
            entity++;
	        sz++;
            continue;
        }
        Vec2i tile = Vec2d_to_Vec2i((*entity)->pos);
        bfs[tile.x][tile.y] += 1000.0;
	    for(int i = 1; i < 9; i++) {
		    Vec2i adjTile = Vec2i_add(tile, (Vec2i){dx[i], dy[i]});
		    if(!isOutOfBounds(adjTile, gState->currentLevel->currentRoom))
			    bfs[adjTile.x][adjTile.y]+=0.5; 
	    }
        entity++;
	    sz++;
    }

    SortedEntity* sortedEntities = calloc(sz + 1, sizeof(SortedEntity));
    entity = entities;
    int ind2 = 0;
    while(*entity != NULL) {
        if(isNotAMonster(*entity)) {
            entity++; ind2++;
            continue;
        }
        sortedEntities[nonProjIndex++] = (SortedEntity){*entity, ind2};
        for(int ind = nonProjIndex - 1; ind > 0; ind--) {
                if(bfsDist(sortedEntities[ind].entity, bfs) < bfsDist(sortedEntities[ind-1].entity, bfs)) {
                        SortedEntity aux = sortedEntities[ind];
                        sortedEntities[ind] = sortedEntities[ind-1];
                        sortedEntities[ind - 1] = aux;
                }
                else
                        break;
        }
        entity++; ind2++;
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
        Vec2d tileCenter = Vec2d_add(Vec2i_to_Vec2d(tile), (Vec2d){0.5, 0.5});

        entityPos->attack_velocity = Vec2d_scale(Vec2d_normalize((Vec2d){start.x + 0.5 - entityPos->pos.x, start.y + 0.5 - entityPos->pos.y}), entityPos->attack_SPD);

        double maxNext = D_INF;
        Vec2i nextPos = tile;
	    int newDir = -1;

        for(int dir = 2; dir < 9; dir += 2) //NSEW
        {
            Vec2i nextTile = Vec2i_add(tile, (Vec2i){dx[dir], dy[dir]});
            if(!isOutOfBounds(nextTile, gState->currentLevel->currentRoom)
            && isWalkable(gState->currentLevel->currentRoom, nextTile))
            {
                if(maxNext > bfs[nextTile.x][nextTile.y]) {
                    maxNext = bfs[nextTile.x][nextTile.y];
                    nextPos = nextTile;
		            newDir = dir;
                }
            }
        }

        for(int dir = 1; dir < 9; dir += 2) //DIAG
        {
            Vec2i nextTile = Vec2i_add(tile, (Vec2i){dx[dir], dy[dir]});
            if(!isOutOfBounds(nextTile, gState->currentLevel->currentRoom)
               && isWalkable(gState->currentLevel->currentRoom, nextTile)
               && isWalkable(gState->currentLevel->currentRoom, Vec2i_add(tile, (Vec2i){dx[dir - 1], dy[dir - 1]}))
               && isWalkable(gState->currentLevel->currentRoom, Vec2i_add(tile, (Vec2i){dx[dir + 1], dy[dir + 1]})))
            {
                if(maxNext > bfs[nextTile.x][nextTile.y]) {
                    maxNext = bfs[nextTile.x][nextTile.y];
                    nextPos = nextTile;
                    newDir = dir;
                    //printf("%d %d in\n", newDir, wtf);
                }
            }
        }

        //printf("%d final\n", newDir);

        int vIndex = sEntity[index].index;

        if(maxNext == D_INF) {
            //printf("xddd\n");
            velocity[vIndex] = (Vec2d){0.0, 0.0};
        }

        else {
            if (newDir % 2 == 0)
            {
                double x_to_centre = fabs(tileCenter.x - hbCenter.x);
                double y_to_centre = fabs(tileCenter.y - hbCenter.y);
                if(newDir % 4 == 2 && y_to_centre > EPSILON) {
                    double y_coord = min(y_to_centre * 60 / 5, entityPos->SPD);
                    if (tileCenter.y < hbCenter.y)
                    {
                        y_coord *= -1;
                    }
                    double x_coord = dx[newDir] * sqrt(fabs(entityPos->SPD * entityPos->SPD - y_coord * y_coord));
                    velocity[vIndex] = (Vec2d){x_coord, y_coord};
                    //printf("%d visited in while loop option 1 , next pos \n", vIndex);
//                    Vec2d_print(Vec2i_to_Vec2d(nextPos));
//                    printf(" curent pos");
//                    Vec2d_print(Vec2d_scale(hbCenter, -1.0f));
//                    printf(", to normalize");
//                    Vec2d_print(Vec2d_add(tileCenter, Vec2d_scale(hbCenter, -1.0f)));
//                    printf(", actual answer ");
//                    Vec2d_print(velocity[vIndex]);
//                    printf("\n");
                }
                else if (newDir % 4 == 0 && x_to_centre > EPSILON)
                {
                    //Vec2d_print(entityPos->pos);
                   // printf(" %d visited in while loop option 2, next pos \n", vIndex);
                    double x_coord = min(x_to_centre * 60 / 5, entityPos->SPD);
                    if (tileCenter.x < hbCenter.x)
                    {
                        x_coord *= -1;
                    }
                    double y_coord = dy[newDir] * sqrt(fabs(entityPos->SPD * entityPos->SPD - x_coord * x_coord));
                    velocity[vIndex] = (Vec2d){x_coord, y_coord};
                }
                else {
                   // Vec2d_print(entityPos->pos);
                 //   printf(" %d visited in while loop option 3 , next pos \n", vIndex);
                    bfs[nextPos.x][nextPos.y] += 1000.0;
                    Vec2d next_centre = Vec2d_add(Vec2i_to_Vec2d(nextPos), (Vec2d){0.5, 0.5});
//                    Vec2d_print(next_centre);
//                    printf("\n");
                    velocity[vIndex] = Vec2d_scale(Vec2d_normalize(Vec2d_add(next_centre, Vec2d_scale(entityPos->pos, -1))), entityPos->SPD);

//                    printf("%d visited in while loop option 2 , next pos ", vIndex);
//                    Vec2d_print(Vec2i_to_Vec2d(nextPos));
//                    printf("\n");
                }
            }
            else
            {
//                printf("%d %d, ", isWalkable(gState->currentLevel->currentRoom, Vec2i_add(tile, (Vec2i){dx[newDir - 1], dy[newDir - 1]})),
//                       isWalkable(gState->currentLevel->currentRoom, Vec2i_add(tile, (Vec2i){dx[newDir + 1], dy[newDir + 1]})));
//                Vec2d_print(entityPos->pos);
//                printf(" %d visited in while loop option 4 , next pos  ", vIndex);
                bfs[nextPos.x][nextPos.y] += 1000.0;
                Vec2d next_centre = Vec2d_add(Vec2i_to_Vec2d(nextPos), (Vec2d){0.5, 0.5});
//                Vec2d_print(next_centre);
//                printf("\n");
                velocity[vIndex] = Vec2d_scale(Vec2d_normalize(Vec2d_add(next_centre, Vec2d_scale(entityPos->pos, -1))), entityPos->SPD);
//                printf("%d visited in while loop option 3, next pos ", vIndex);
//                Vec2d_print(Vec2i_to_Vec2d(nextPos));
//                printf(" curent pos");
//                Vec2d_print(Vec2i_to_Vec2d(Vec2i_scale(tile, -1)));
//                printf(", to normalize");
//                Vec2d_print(Vec2i_to_Vec2d(Vec2i_add(nextPos, Vec2i_scale(tile, -1))));
//                printf(", actual answer ");
//                Vec2d_print(velocity[vIndex]);
//                printf("\n");
            }
        }
        index++;
    }

    for(int i = 0; i < gState->currentLevel->currentRoom->size.x; i++) {
       free(bfs[i]);
    }

    pqueue_delete(pq);
    free(bfs);
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
    gState->currentLevel->currentRoom = calloc(1, sizeof(Room));
    Vec2d playerPos = (Vec2d){12, 12};
    Entity entity = Entity_construct();
    Entity entity2 = Entity_construct();
    entity2.pos = (Vec2d){13, 14};
    gState->currentLevel->currentRoom->tiles[1][2].type = TILE_WALL;
    gState->currentLevel->currentRoom->tiles[2][1].type = TILE_WALL;
    Entity** entities = calloc(3, sizeof(Entity));
    entities[0] = &entity;
    entities[1] = &entity2;
    Vec2d* paths = path(playerPos, entities, gState);
    for(int i = 0; i < 3; i++) {
	printf("%f %f\n", paths[i].x, paths[i].y);
    }
}
*/
