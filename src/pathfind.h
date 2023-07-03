#ifndef PATHFIND_H
#define PATHFIND_H

#include "game_math.h"
#include "entity.h"
#include "state.h"

void init_pathfind();
void recompute_jps_data(Room *room);
void path(Vec2d playerPosition, Entity** entities, GameState* gState);

#endif //PATHFIND_H
