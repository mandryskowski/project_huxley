#ifndef PATHFIND_H
#define PATHFIND_H

#include "math.h"
#include "entity.h"
#include "state.h"

Vec2f* path(Vec2f playerPosition, Entity** entities, GameState* gState);

#endif //PATHFIND_H
