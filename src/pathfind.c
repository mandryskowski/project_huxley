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
#define D_INF 1e9
#define SQRT2 1.41421356237
typedef struct SortedEntity {
	Entity* entity;
	int index;
} SortedEntity;

typedef struct PQElement {
    int dir;
	double value;
	Vec2i tile;
} PQElement;

PQElement *newPQElement(int dir, double value, Vec2i tile) {
    PQElement* newEl = malloc(sizeof(PQElement));
    *newEl = (PQElement){dir, value, tile};
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
    return ((PQElement*)_this)->value < ((PQElement*)_other)->value;
}

//Vec2d* path(Vec2d start, Entity** entities, GameState* gState) {
//
//    Vec2i startTile = Vec2d_to_Vec2i(start);
//    int MAX_SIZE = gState->currentLevel->currentRoom->size.x * gState->currentLevel->currentRoom->size.y;
//
//    double** bfs = calloc(gState->currentLevel->currentRoom->size.x, sizeof(double*));
//    for(int i = 0; i < gState->currentLevel->currentRoom->size.x; i++) {
//        bfs[i] = calloc(gState->currentLevel->currentRoom->size.y, sizeof(double));
//        for(int j = 0; j < gState->currentLevel->currentRoom->size.y; j++) {
//            bfs[i][j] = D_INF;
//        }
//    }
//
//    bfs[startTile.x][startTile.y] = 20000.0;
//
//    PQueue* pq = pqueue_new(_cmp, MAX_SIZE);
//    pqueue_enqueue(pq, newPQElement(20000.0, startTile));
// // Zombie dijkstra
//    while(pq->size) {
//        PQElement* pqtop = (PQElement*)pqueue_dequeue(pq);
//	    Vec2i currTile = pqtop->tile;
//        if (bfs[currTile.x][currTile.y] < pqtop->value)
//        {
//            continue;
//        }
//        for(int dir = 1; dir < 9; dir += 1) { //NSWE
//            Vec2i nextTile = Vec2i_add(currTile, (Vec2i){dx[dir], dy[dir]});
//            if(isWalkable(gState->currentLevel->currentRoom, nextTile)
//                && !(dir % 2 == 1
//                    && (!isWalkable(gState->currentLevel->currentRoom, Vec2i_add(currTile, (Vec2i){dx[dir - 1], dy[dir - 1]}))
//                    || !isWalkable(gState->currentLevel->currentRoom, Vec2i_add(currTile, (Vec2i){dx[dir + 1], dy[dir + 1]}))))
//                )
//            {
//                double dValue = ((dir%2) ? 1.4 : 1.0) + bfs[currTile.x][currTile.y];
//                if(dValue < bfs[nextTile.x][nextTile.y]) {
//                    bfs[nextTile.x][nextTile.y] = dValue;
//                    pqueue_enqueue(pq, newPQElement(dValue, nextTile));
//                }
//            }
//        }
//    }
//    bfs[startTile.x][startTile.y] = 0.0;
//
//    // TRY TO INCENTIVISE GOING AROUND OTHER ENEMIES
//    int sz = 0, nonProjIndex = 0;
//    Entity** entity = entities;
//    while(*entity != NULL) {
//        if (isNotAMonster(*entity)) {
//            entity++;
//	        sz++;
//            continue;
//        }
//        Vec2i tile = Vec2d_to_Vec2i((*entity)->pos);
//        bfs[tile.x][tile.y] += 1000.0;
//	    for(int i = 1; i < 9; i++) {
//		    Vec2i adjTile = Vec2i_add(tile, (Vec2i){dx[i], dy[i]});
//		    if(!isOutOfBounds(adjTile, gState->currentLevel->currentRoom))
//			    bfs[adjTile.x][adjTile.y]+=0.5;
//	    }
//        entity++;
//	    sz++;
//    }
//
//    SortedEntity* sortedEntities = calloc(sz + 1, sizeof(SortedEntity));
//    entity = entities;
//    int ind2 = 0;
//    while(*entity != NULL) {
//        if(isNotAMonster(*entity)) {
//            entity++; ind2++;
//            continue;
//        }
//        sortedEntities[nonProjIndex++] = (SortedEntity){*entity, ind2};
//        for(int ind = nonProjIndex - 1; ind > 0; ind--) {
//                if(bfsDist(sortedEntities[ind].entity, bfs) < bfsDist(sortedEntities[ind-1].entity, bfs)) {
//                        SortedEntity aux = sortedEntities[ind];
//                        sortedEntities[ind] = sortedEntities[ind-1];
//                        sortedEntities[ind - 1] = aux;
//                }
//                else
//                        break;
//        }
//        entity++; ind2++;
//    }
//
//
//
//    Vec2d* velocity = calloc(sz + 1, sizeof(Vec2d));
//    for(int i = 0; i < sz; i++)
//	velocity[i] = (Vec2d){0.0, 0.0};
//
//    SortedEntity* sEntity = sortedEntities;
//    int index = 0;
//    while(index < nonProjIndex) {
//        Entity* entityPos = sEntity[index].entity;
//
//        Vec2d hbCenter = Vec2d_add(Vec2d_center(entityPos->hitbox), entityPos->pos);
//
//        Vec2i tile = Vec2d_to_Vec2i(entityPos->pos);
//        Vec2d tileCenter = Vec2d_add(Vec2i_to_Vec2d(tile), (Vec2d){0.5, 0.5});
//
//        Vec2d player_pred  = gState->player->entity->pos;
//        double dist_to_player;
//        double travel_time;
//        for (int i = 0; i < 5; i++)
//        {
//            dist_to_player = Vec2d_metric_distance(player_pred, entityPos->pos);
//            travel_time = (dist_to_player) / entityPos->attack_SPD;
//            player_pred = Vec2d_add(gState->player->entity->pos, Vec2d_scale(gState->player->entity->velocity, travel_time));
//        }
//        player_pred = Vec2d_add(player_pred, (Vec2d){-(double)(rand() % 2) / 10, -(double)(rand() % 2) / 10});
//        entityPos->attack_velocity = Vec2d_scale(Vec2d_normalize((Vec2d){player_pred.x - entityPos->pos.x, player_pred.y - entityPos->pos.y}), entityPos->attack_SPD);
//
//        double maxNext = D_INF;
//        Vec2i nextPos = tile;
//	    int newDir = -1;
//
//        for(int dir = 2; dir < 9; dir += 2) //NSEW
//        {
//            Vec2i nextTile = Vec2i_add(tile, (Vec2i){dx[dir], dy[dir]});
//            if(!isOutOfBounds(nextTile, gState->currentLevel->currentRoom)
//            && isWalkable(gState->currentLevel->currentRoom, nextTile))
//            {
//                if(maxNext > bfs[nextTile.x][nextTile.y]) {
//                    maxNext = bfs[nextTile.x][nextTile.y];
//                    nextPos = nextTile;
//		            newDir = dir;
//                }
//            }
//        }
//
//        for(int dir = 1; dir < 9; dir += 2) //DIAG
//        {
//            Vec2i nextTile = Vec2i_add(tile, (Vec2i){dx[dir], dy[dir]});
//            if(!isOutOfBounds(nextTile, gState->currentLevel->currentRoom)
//               && isWalkable(gState->currentLevel->currentRoom, nextTile)
//               && isWalkable(gState->currentLevel->currentRoom, Vec2i_add(tile, (Vec2i){dx[dir - 1], dy[dir - 1]}))
//               && isWalkable(gState->currentLevel->currentRoom, Vec2i_add(tile, (Vec2i){dx[dir + 1], dy[dir + 1]})))
//            {
//                if(maxNext > bfs[nextTile.x][nextTile.y]) {
//                    maxNext = bfs[nextTile.x][nextTile.y];
//                    nextPos = nextTile;
//                    newDir = dir;
//                    //printf("%d %d in\n", newDir, wtf);
//                }
//            }
//        }
//
//        //printf("%d final\n", newDir);
//
//        int vIndex = sEntity[index].index;
//
//        if(maxNext == D_INF) {
//            //printf("xddd\n");
//            velocity[vIndex] = (Vec2d){0.0, 0.0};
//        }
//
//        else {
//            if (newDir % 2 == 0)
//            {
//                double x_to_centre = fabs(tileCenter.x - hbCenter.x);
//                double y_to_centre = fabs(tileCenter.y - hbCenter.y);
//                if(newDir % 4 == 2 && y_to_centre > EPSILON) {
//                    double y_coord = min(y_to_centre * 60 / 5, entityPos->SPD);
//                    if (tileCenter.y < hbCenter.y)
//                    {
//                        y_coord *= -1;
//                    }
//                    double x_coord = dx[newDir] * sqrt(fabs(entityPos->SPD * entityPos->SPD - y_coord * y_coord));
//                    velocity[vIndex] = (Vec2d){x_coord, y_coord};
//                    //printf("%d visited in while loop option 1 , next pos \n", vIndex);
////                    Vec2d_print(Vec2i_to_Vec2d(nextPos));
////                    printf(" curent pos");
////                    Vec2d_print(Vec2d_scale(hbCenter, -1.0f));
////                    printf(", to normalize");
////                    Vec2d_print(Vec2d_add(tileCenter, Vec2d_scale(hbCenter, -1.0f)));
////                    printf(", actual answer ");
////                    Vec2d_print(velocity[vIndex]);
////                    printf("\n");
//                }
//                else if (newDir % 4 == 0 && x_to_centre > EPSILON)
//                {
//                    //Vec2d_print(entityPos->pos);
//                   // printf(" %d visited in while loop option 2, next pos \n", vIndex);
//                    double x_coord = min(x_to_centre * 60 / 5, entityPos->SPD);
//                    if (tileCenter.x < hbCenter.x)
//                    {
//                        x_coord *= -1;
//                    }
//                    double y_coord = dy[newDir] * sqrt(fabs(entityPos->SPD * entityPos->SPD - x_coord * x_coord));
//                    velocity[vIndex] = (Vec2d){x_coord, y_coord};
//                }
//                else {
//                   // Vec2d_print(entityPos->pos);
//                 //   printf(" %d visited in while loop option 3 , next pos \n", vIndex);
//                    bfs[nextPos.x][nextPos.y] += 1000.0;
//                    Vec2d next_centre = Vec2d_add(Vec2i_to_Vec2d(nextPos), (Vec2d){0.5, 0.5});
////                    Vec2d_print(next_centre);
////                    printf("\n");
//                    velocity[vIndex] = Vec2d_scale(Vec2d_normalize(Vec2d_add(next_centre, Vec2d_scale(entityPos->pos, -1))), entityPos->SPD);
//
////                    printf("%d visited in while loop option 2 , next pos ", vIndex);
////                    Vec2d_print(Vec2i_to_Vec2d(nextPos));
////                    printf("\n");
//                }
//            }
//            else
//            {
////                printf("%d %d, ", isWalkable(gState->currentLevel->currentRoom, Vec2i_add(tile, (Vec2i){dx[newDir - 1], dy[newDir - 1]})),
////                       isWalkable(gState->currentLevel->currentRoom, Vec2i_add(tile, (Vec2i){dx[newDir + 1], dy[newDir + 1]})));
////                Vec2d_print(entityPos->pos);
////                printf(" %d visited in while loop option 4 , next pos  ", vIndex);
//                bfs[nextPos.x][nextPos.y] += 1000.0;
//                Vec2d next_centre = Vec2d_add(Vec2i_to_Vec2d(nextPos), (Vec2d){0.5, 0.5});
////                Vec2d_print(next_centre);
////                printf("\n");
//                velocity[vIndex] = Vec2d_scale(Vec2d_normalize(Vec2d_add(next_centre, Vec2d_scale(entityPos->pos, -1))), entityPos->SPD);
////                printf("%d visited in while loop option 3, next pos ", vIndex);
////                Vec2d_print(Vec2i_to_Vec2d(nextPos));
////                printf(" curent pos");
////                Vec2d_print(Vec2i_to_Vec2d(Vec2i_scale(tile, -1)));
////                printf(", to normalize");
////                Vec2d_print(Vec2i_to_Vec2d(Vec2i_add(nextPos, Vec2i_scale(tile, -1))));
////                printf(", actual answer ");
////                Vec2d_print(velocity[vIndex]);
////                printf("\n");
//            }
//        }
//        index++;
//    }
//
//    for(int i = 0; i < gState->currentLevel->currentRoom->size.x; i++) {
//       free(bfs[i]);
//    }
//
//    pqueue_delete(pq);
//    free(bfs);
//    free(sortedEntities);
//    return velocity;
//}

static int jps_cnt;
static int jps_data[8][30][30];
static double dist[30][30];
static int start_dir[30][30];
static int visited_cnt;
static Vec2i cards[8] = {(Vec2i){-1, 1}, (Vec2i){0, 1}, (Vec2i){1, 1}, (Vec2i){1, 0}, (Vec2i){1, -1}, (Vec2i){0, -1}, (Vec2i){-1, -1}, (Vec2i){-1, 0}};
static Vec2i visited[900];
static PQueue* pq;

void init_pathfind()
{
    pq = pqueue_new(_cmp, 900);
    for(int i = 0; i < 30; i++) {
        for(int j = 0; j < 30; j++) {
            dist[i][j] = D_INF;
        }
    }
}

void recompute_jps_data(Room *room)
{
    jps_cnt = 0;
    bool jmp_flags[4][room->size.x][room->size.y];
    Vec2i dirs[4] = {(Vec2i){-1, 1}, (Vec2i){1, 1}, (Vec2i){-1, -1}, (Vec2i){1, -1}};

    for (int i = 0; i < room->size.x; i++)
        for (int j = 0; j < room->size.y; j++)
            for (int k = 0; k < 4; k++)
                jmp_flags[k][i][j] = false;

    for (int i = 1; i < room->size.x - 1; i++)
    {
        for (int j = 1; j < room->size.y - 1; j++)
        {
            if (getTile((Vec2i){i, j}, room) != TILE_FLOOR)
            {
                continue;
            }
            bool is_jp = false;
            for (int k = 0; k < 4; k++)
            {
                Vec2i tile = Vec2i_add((Vec2i){i, j}, dirs[k]);
                Vec2i left = Vec2i_add((Vec2i){i, j}, (Vec2i){dirs[k].x, 0});
                Vec2i right = Vec2i_add((Vec2i){i, j}, (Vec2i){0, dirs[k].y});
                if (getTile(tile, room) != TILE_FLOOR && getTile(left, room) == TILE_FLOOR
                 && getTile(right, room) == TILE_FLOOR)
                {
                    jmp_flags[dirs[k].x > 0 ? 1 : 2][i][j] = true;
                    jmp_flags[dirs[k].y > 0 ? 0 : 3][i][j] = true;
                    if (!is_jp)
                    {
                        jps_cnt++;
                        is_jp = true;
                    }
                }
            }
//            for (int k = 0; k < 4; k++)
//            {
//                if (jmp_flags[k][i][j] > 0)
//                {
//                    printf("%d %d %d jps k i j\n", k, i, j);
//                }
//            }
        }
    }

    for (int i = 0; i < room->size.x; i++)
        for (int k = 0; k < 8; k++)
            jps_data[k][i][room->size.y - 1] = 0;

    for (int i = 0; i < room->size.y; i++)
        for (int k = 0; k < 8; k++)
            jps_data[k][room->size.x - 1][i] = 0;

    // left up
    for (int i = 1; i < room->size.x - 1; i++)
    {
        for (int j = 1; j < room->size.y - 1; j++)
        {
            if (getTile((Vec2i){i, j}, room) != TILE_FLOOR)
            {
                jps_data[3][i][j] = 0;
                jps_data[1][i][j] = 0;
                continue;
            }
            if (jmp_flags[1][i - 1][j])
                jps_data[7][i][j] = 1;
            else
                jps_data[7][i][j] = jps_data[7][i - 1][j] > 0 ? jps_data[7][i - 1][j] + 1 : jps_data[7][i - 1][j] - 1;

            if (jmp_flags[0][i][j - 1])
                jps_data[5][i][j] = 1;
            else
                jps_data[5][i][j] = jps_data[5][i][j - 1] > 0 ? jps_data[5][i][j - 1] + 1 : jps_data[5][i][j - 1] - 1;
        }
    }

    // right down
    for (int i = room->size.x - 1; i > 0; i--)
    {
        for (int j = room->size.y - 1; j > 0; j--)
        {
            if (getTile((Vec2i){i, j}, room) != TILE_FLOOR)
            {
                jps_data[7][i][j] = 0;
                jps_data[5][i][j] = 0;
                continue;
            }
            if (jmp_flags[2][i + 1][j])
                jps_data[3][i][j] = 1;
            else
                jps_data[3][i][j] = jps_data[3][i + 1][j] > 0 ? jps_data[3][i + 1][j] + 1 : jps_data[3][i + 1][j] - 1;

            if (jmp_flags[3][i][j + 1])
                jps_data[1][i][j] = 1;
            else
                jps_data[1][i][j] = jps_data[1][i][j + 1] > 0 ? jps_data[1][i][j + 1] + 1 : jps_data[1][i][j + 1] - 1;
        }
    }

    //diag down
    for (int i = room->size.x - 2; i > 0; i--)
    {
        for (int j = 1; j < room->size.y - 1; j++)
        {
            if (getTile((Vec2i){i, j}, room) != TILE_FLOOR)
            {
                jps_data[4][i][j] = 0;
                jps_data[2][i][j] = 0;
                continue;
            }
            if (getTile((Vec2i){i + 1, j}, room) != TILE_FLOOR || getTile((Vec2i){i, j - 1}, room) != TILE_FLOOR)
                jps_data[4][i][j] = -1;
            else if (jps_data[3][i + 1][j - 1] > 0 || jps_data[5][i + 1][j - 1] > 0)
                jps_data[4][i][j] = 1;
            else
                jps_data[4][i][j] = jps_data[4][i + 1][j - 1] > 0 ? jps_data[4][i + 1][j - 1] + 1 : jps_data[4][i + 1][j - 1] - 1;

            if (getTile((Vec2i){i + 1, j}, room) != TILE_FLOOR || getTile((Vec2i){i, j + 1}, room) != TILE_FLOOR)
                jps_data[2][i][j] = -1;
            else if (jps_data[1][i + 1][j + 1] > 0 || jps_data[3][i + 1][j + 1] > 0)
                jps_data[2][i][j] = 1;
            else
                jps_data[2][i][j] = jps_data[2][i + 1][j + 1] > 0 ? jps_data[2][i + 1][j + 1] + 1 : jps_data[2][i + 1][j + 1] - 1;
        }
    }

    //diag up
    for (int i = 1; i < room->size.x - 1; i++)
    {
        for (int j = 1; j < room->size.y - 1; j++)
        {
            if (getTile((Vec2i){i, j}, room) != TILE_FLOOR)
            {
                jps_data[6][i][j] = 0;
                jps_data[0][i][j] = 0;
                continue;
            }
            if (getTile((Vec2i){i, j - 1}, room) != TILE_FLOOR || getTile((Vec2i){i - 1, j}, room) != TILE_FLOOR)
                jps_data[6][i][j] = -1;
            else if (jps_data[5][i - 1][j - 1] > 0 || jps_data[7][i - 1][j - 1] > 0)
                jps_data[6][i][j] = 1;
            else
                jps_data[6][i][j] = jps_data[6][i - 1][j - 1] > 0 ? jps_data[6][i - 1][j - 1] + 1 : jps_data[6][i - 1][j - 1] - 1;

            if (getTile((Vec2i){i, j + 1}, room) != TILE_FLOOR || getTile((Vec2i){i - 1, j}, room) != TILE_FLOOR)
                jps_data[0][i][j] = -1;
            else if (jps_data[7][i - 1][j + 1] > 0 || jps_data[1][i - 1][j + 1] > 0)
                jps_data[0][i][j] = 1;
            else
                jps_data[0][i][j] = jps_data[0][i - 1][j + 1] > 0 ? jps_data[0][i - 1][j + 1] + 1 : jps_data[0][i - 1][j + 1] - 1;
        }
    }
}

double get_heuristic(Vec2i tile, Vec2i dest)
{
    Vec2i diff = Vec2i_sub(tile, dest);
    diff = (Vec2i){abs(diff.x), abs(diff.y)};
    return (sqrt(2) - 1) * min(diff.x, diff.y) + max(diff.x, diff.y);
}

void add_jp(int dir, Vec2i pos, Vec2i dest)
{
    int dist_to_jp = jps_data[dir][pos.x][pos.y];
    Vec2i dist_to_end = Vec2i_sub(dest, pos);
    if (dir % 2 == 0)
    {
        int min_coord = min(abs(dist_to_end.x), abs(dist_to_end.y));
        //printf("maybe... curr pos: (%d, %d); dist_to_end: (%d, %d); dir: (%d, %d); jps %d; min_coord %d\n", pos.x, pos.y, dist_to_end.x, dist_to_end.y, cards[dir].x, cards[dir].y, jps_data[dir][pos.x][pos.y], min_coord);
        //printf("x: %d; y: %d; jps: %d\n", cards[dir].x * dist_to_end.x > 0, cards[dir].y * dist_to_end.y > 0, jps_data[dir][pos.x][pos.y] > min_coord);
        if (cards[dir].x * dist_to_end.x > 0 && cards[dir].y * dist_to_end.y > 0
          && abs(jps_data[dir][pos.x][pos.y]) > min_coord)
        {
            //printf("curr pos: (%d, %d); dest: (%d, %d); dir: (%d, %d)\n", pos.x, pos.y, dest.x, dest.y, cards[dir].x, cards[dir].y);
            dist_to_jp = min_coord;
        }
    }
    else
    {
        Vec2i dist_norm = Vec2i_normalize(dist_to_end);
        int min_coord = dist_to_end.x ? abs(dist_to_end.x) : abs(dist_to_end.y);
        //printf("%d %d try to deset, %d %d dist_norm, %d jps and %d min_coord, %d dir\n", cards[dir].x, cards[dir].y, dist_norm.x, dist_norm.y, jps_data[dir][pos.x][pos.y], min_coord, dir);
        if (dist_norm.x == cards[dir].x && dist_norm.y == cards[dir].y &&  abs(jps_data[dir][pos.x][pos.y]) > min_coord)
        {
            dist_to_jp = min_coord;
           // printf("xdd\n");
        }
    }
    if (dist_to_jp <= 0)
        return;
    double mp = dir % 2 ? 1 : SQRT2;
    Vec2i new_pos = Vec2i_add(pos, Vec2i_scale(cards[dir], dist_to_jp));
    double new_dist = dist_to_jp * mp + dist[pos.x][pos.y];
    if (dist[new_pos.x][new_pos.y] > new_dist)
    {
        if (dist[new_pos.x][new_pos.y] == D_INF)
        {
            visited[visited_cnt++] = new_pos;
        }
     //   printf("%d %d %f enqueue, old pos %d %d, new dist %f\n", new_pos.x, new_pos.y, new_dist + get_heuristic(new_pos, dest), pos.x, pos.y, new_dist);
        dist[new_pos.x][new_pos.y] = new_dist;
        start_dir[new_pos.x][new_pos.y] = start_dir[pos.x][pos.y];
        pqueue_enqueue(pq, newPQElement(dir, new_dist + get_heuristic(new_pos, dest), new_pos));
    }
//    else
//    {
//        printf("dist[new_pos.x][new_pos.y]: %f; new_dist: %f\n", dist[new_pos.x][new_pos.y], new_dist);
//    }
}

int JPS(Vec2d dest, Entity *monster)
{
//    for (int k = 0; k < 8; k++)
//    {
//        printf("(%d, %d)\n", cards[k].x, cards[k].y);
//        for (int i = 1; i < monster->room->size.x - 1; i++)
//        {
//            for (int j = 1; j < monster->room->size.y - 1; j++) {
//                printf("%d ", jps_data[k][i][j]);
//            }
//            printf("\n");
//        }
//        printf("\n");
//    }
//    printf("%d\n", jps_data[2][16][8]);
//    exit(0);
//    for (int i = 0 ; i < monster->room->size.x; i++)
//    {
//        for (int j = 0; j < monster->room->size.y; j++)
//        {
//            if (dist[i][j] != D_INF)
//            {
//                printf("LULE\n");
//            }
//        }
//    }
    //printf("start _____________________________________\n");
//    Vec2d_print(monster->pos);
//    printf("\n");
    Vec2i dest_i = Vec2d_to_Vec2i(dest);
    Vec2i start = Vec2d_to_Vec2i(monster->pos);

    dist[start.x][start.y] = 0;
    visited[visited_cnt++] = start;

    for (int i = 0; i < 8; i++)
    {
        start_dir[start.x][start.y] = i;
        add_jp(i, start, dest_i);
    }

    bool finished = false;

    while(pq->size) {
        PQElement* pqtop = (PQElement*)pqueue_dequeue(pq);
       // printf("%d %d, visited\n", pqtop->tile.x, pqtop->tile.y);
        Vec2i currTile = pqtop->tile;

        if (currTile.x == dest_i.x && currTile.y == dest_i.y)
            finished = true;

        if (finished || dist[currTile.x][currTile.y] + EPSILON + get_heuristic(pqtop->tile, dest_i) < pqtop->value)
        {
            //printf("%f %f dist not fresh\n", dist[currTile.x][currTile.y] + EPSILON + get_heuristic(pqtop->tile, dest_i), pqtop->value);
            free(pqtop);
            continue;
        }

        add_jp(pqtop->dir, currTile, dest_i);
       // printf("current dir: (%d, %d)\n", cards[pqtop->dir].x, cards[pqtop->dir].y);

        // non diagonal
        if (pqtop->dir % 2)
        {
            //printf("dir %d\n", pqtop->dir);
            if (jps_data[(pqtop->dir + 5) % 8][currTile.x][currTile.y])
            {
                add_jp((pqtop->dir + 6) % 8, currTile, dest_i);
                add_jp((pqtop->dir + 7) % 8, currTile, dest_i);
            }
            if (jps_data[(pqtop->dir + 3) % 8][currTile.x][currTile.y])
            {
                add_jp((pqtop->dir + 1) % 8, currTile, dest_i);
                add_jp((pqtop->dir + 2) % 8, currTile, dest_i);
            }
        }
        else
        {
         //   printf("xdd\n");
            add_jp((pqtop->dir + 7) % 8, currTile, dest_i);
            add_jp((pqtop->dir + 1) % 8, currTile, dest_i);
        }
        free(pqtop);
    }

    int ans = start_dir[dest_i.x][dest_i.y];
    while (visited_cnt)
    {
        dist[visited[visited_cnt - 1].x][visited[visited_cnt - 1].y] = D_INF;
        visited_cnt--;
    }
   // printf("res: %d %d\nend _____________________________________\n", cards[ans].x, cards[ans].y);
    return ans;

}

void path(Vec2d dest, Entity** entities, GameState* gState)
{
    for (Entity **monsterp = entities; *monsterp; monsterp++)
    {
        Entity *monster = *monsterp;
        if (isNotAMonster(monster))
            continue;

        if (monster->pos.x == dest.x && monster->pos.y == dest.y)
        {
            monster->velocity = Vec2d_scale(Vec2d_normalize(Vec2d_sub(dest, monster->pos)), monster->SPD);
            return;
        }

        int dir = JPS(dest, monster);
        Vec2d hbCenter = Vec2d_add(Vec2d_center(monster->hitbox), monster->pos);
        Vec2d tileCenter = Vec2d_add(Vec2i_to_Vec2d(Vec2d_to_Vec2i(monster->pos)), (Vec2d){0.5, 0.5});

        if (dir % 2 == 1)
        {
            double x_to_centre = fabs(tileCenter.x - hbCenter.x);
            double y_to_centre = fabs(tileCenter.y - hbCenter.y);
            double x_coord, y_coord;

            if (dir % 4 == 3) {
                y_coord = min(y_to_centre * 60 / 5, monster->SPD);
                if (tileCenter.y < hbCenter.y)
                {
                    y_coord *= -1;
                }
                x_coord = cards[dir].x * sqrt(fabs(monster->SPD * monster->SPD - y_coord * y_coord));
                monster->velocity = (Vec2d){x_coord, y_coord};
                Vec2d_print(monster->velocity);
            }
            else
            {
                x_coord = min(x_to_centre * 60 / 5, monster->SPD);
                if (tileCenter.x < hbCenter.x)
                {
                    x_coord *= -1;
                }
                y_coord = cards[dir].y * sqrt(fabs(monster->SPD * monster->SPD - x_coord * x_coord));

            }
            monster->velocity = (Vec2d){x_coord, y_coord};
        }
        else
        {
            Vec2d next_centre = Vec2d_add(tileCenter, Vec2i_to_Vec2d(cards[dir]));
            monster->velocity = Vec2d_scale(Vec2d_normalize(Vec2d_sub(next_centre, monster->pos)), monster->SPD);
        }

        Vec2d player_pred  = gState->player->entity->pos;
        double dist_to_player;
        double travel_time;
        for (int i = 0; i < 5; i++)
        {
            dist_to_player = Vec2d_metric_distance(player_pred, monster->pos);
            travel_time = (dist_to_player) / monster->attack_SPD;
            player_pred = Vec2d_add(gState->player->entity->pos, Vec2d_scale(gState->player->entity->velocity, travel_time));
        }
        player_pred = Vec2d_add(player_pred, (Vec2d){-(double)(rand() % 2) / 10, -(double)(rand() % 2) / 10});
        monster->attack_velocity = Vec2d_scale(Vec2d_normalize((Vec2d){player_pred.x - monster->pos.x, player_pred.y - monster->pos.y}), monster->attack_SPD);

        // printf("dir: %d %d; new vel: %f %f\n", cards[dir].x, cards[dir].y, monster->velocity.x, monster->velocity.y);
    }
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
