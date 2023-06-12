#ifndef PATHFIND_H
#define PATHFIND_H

#include "game_math.h"
#include "entity.h"
#include "state.h"

Vec2d* path(Vec2d playerPosition, Entity** entities, GameState* gState);

#endif //PATHFIND_H
