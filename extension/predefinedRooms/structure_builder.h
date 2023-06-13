#ifndef STRUCTURE_BUILDER_H
#define STRUCTURE_BUILDER_H

#include "../room.h"
#include "../entity.h"
#include "../game_math.h"

typedef enum {FILL,CHECKERED} Pattern;
typedef enum {L_SHAPE, U_SHAPE, T_SHAPE} Structure;

void patternBuilder(TileType** tiles,Pattern pattern, Vec2i topLeft, Vec2i bottomRight, TileType type);
void presetStructures(TileType** tiles, Structure structure, Vec2i topLeft, Vec2i bottomRight, TileType type, int angle);

#endif // STRUCTURE_BUILDER_H